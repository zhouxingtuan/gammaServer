//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/6/5
// Time: 下午3:59
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__client__
#define __hive__client__

//#define NS_HIVE_BEGIN namespace hivenet {
//#define NS_HIVE_END };
//#define USING_NS_HIVE using namespace hivenet;
#define NS_HIVE_BEGIN
#define NS_HIVE_END
#define USING_NS_HIVE

//c libs
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>  //atof函数
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>

//std libs
#include <atomic>
#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>

//socket & net libs
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>		/* basic system data types */
#include <sys/socket.h>		/* basic socket definitions */
#include <netinet/in.h>		/* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>		/* inet(3) functions */
#include <fcntl.h>			/* nonblocking */

//thread libs
#include <pthread.h>
#include <semaphore.h>

#define SAFE_DELETE(ptr) if(ptr != NULL){ delete ptr; ptr = NULL; }
#define SAFE_DELETE_ARRAY(ptr) if(ptr != NULL){ delete [] ptr; ptr = NULL; }
#define SAFE_RETAIN(ptr) if(ptr != NULL){ ptr->retain(); }
#define SAFE_RELEASE(ptr) if(ptr != NULL){ ptr->release(); ptr = NULL; }

NS_HIVE_BEGIN

/*--------------------------------------------------------------------*/
// 拥有同步互斥量的类
class Sync
{
public:
	Sync(void){
		pthread_mutex_init(&m_mutex, NULL);
	}
	virtual ~Sync(void){
		pthread_mutex_destroy(&m_mutex);
	}
	inline void lock(void){
		pthread_mutex_lock(&m_mutex);
	}
	inline void rlock(void){
		pthread_mutex_lock(&m_mutex);
	}
	inline void unlock(void){
		pthread_mutex_unlock(&m_mutex);
	}
protected:
	pthread_mutex_t m_mutex;
};
/*--------------------------------------------------------------------*/
// 引用计数基类
class RefObject
{
public:
	RefObject(void) : m_referenceCount(0){}
	virtual ~RefObject(void){}
	inline void release(void){
		if( std::atomic_fetch_sub_explicit(&m_referenceCount, 1, std::memory_order_relaxed) == 1 ){
			delete this;
		}
	}
	inline void retain(void){
		std::atomic_fetch_add_explicit(&m_referenceCount, 1, std::memory_order_relaxed);
	}
	inline int getRefCount(void){ return (int)m_referenceCount; }
protected:
	std::atomic<int> m_referenceCount;
};
/*--------------------------------------------------------------------*/
// 线程基类
class Thread
{
public:
	typedef void*(*ThreadCallback)(void* pData);
public:
	Thread(void) : m_pThread(0){}
	virtual ~Thread(void){}
	bool startThread(void);
	void cancelThread(void);
	virtual int threadFunction(void) = 0;

	static pthread_t staticThread(ThreadCallback start_rtn, void *arg);
private:
	static void* staticThreadFunction(void* pData);
protected:
	pthread_t m_pThread;
};
/*--------------------------------------------------------------------*/
typedef std::vector<char> CharVector;

class Buffer : public RefObject, public CharVector
{
public:
	Buffer(void) : RefObject(), CharVector(){}
	virtual ~Buffer(void){}

	inline int write(const void* ptr, int length, int offset){
		int needSize = offset + length;
		if( (int)this->size() < needSize ){
			this->resize(needSize, 0);
		}
		char* writeTo = (char*)(this->data()) + offset;
		memcpy(writeTo, ptr, length);
		return length;
	}
	inline int read(void* ptr, int length, int offset){
		int needSize = offset + length;
		if( (int)this->size() < needSize){
			return 0;
		}
		char* readFrom = (char*)(this->data()) + offset;
		memcpy(ptr, readFrom, length);
		return length;
	}
protected:

};// end class Buffer
/*--------------------------------------------------------------------*/
// 服务间传递消息的头部数据结构
typedef struct PacketHead {
	union {
		unsigned int length 	: 24;	// 数据长度
		unsigned int command 	: 8;	// 当前数据包的命令
	};
	unsigned int destination;			// 4 目标服务句柄
} PacketHead;

class Packet : public RefObject
{
public:
    explicit Packet(int bufferSize);
    explicit Packet(Buffer* pBuffer);
    virtual ~Packet(void);

	inline PacketHead* getHead(void){ return (PacketHead*)(getDataPtr()); }	// 该指针在write调用后可能会变化
	inline char* getBody(void){ return getOffsetPtr(sizeof(PacketHead)); }
	inline int getBodyLength(void) const { return (getLength() - sizeof(PacketHead)); }
	inline void setCursor(int cur){ m_cursor = cur; }
	inline void moveCursor(int length){ m_cursor += length; }
	inline int getCursor(void) const { return m_cursor; }
	inline void resetCursor(void){ m_cursor = 0; }
	inline Buffer* getBuffer(void){ return m_pBuffer; }
	inline char* getCursorPtr(void){ return m_pBuffer->data() + m_cursor; }
	inline int getLength(void) const { return (int)m_pBuffer->size(); }
	inline bool isCursorEnd(void) const { return getCursor() >= getLength(); }
	inline int write(const void* ptr, int length){
		int n = m_pBuffer->write(ptr, length, getCursor());
		if( n > 0 ){
			moveCursor(n);
		}
		return n;
	}
	inline int read(void* ptr, int length){
		int n = m_pBuffer->read(ptr, length, getCursor());
		if( n > 0 ){
			moveCursor(n);
		}
		return n;
	}

	inline void readBegin(void){
		setCursor((int)sizeof(PacketHead));
	}
	inline void readEnd(void){
		resetCursor();
	}
	inline void writeBegin(unsigned int cmd, unsigned int handle){
		m_pBuffer->resize(sizeof(PacketHead), 0);
		setCursor((int)sizeof(PacketHead));
		setCommand(cmd);
		setDestination(handle);
	}
	inline void writeEnd(void){
		recordLength();
	}
	inline void setCommand(unsigned int cmd){ getHead()->command = cmd; }
	inline void setDestination(unsigned int handle){ getHead()->destination = handle; }
	inline void recordLength(void){ getHead()->length = getLength(); }
protected:
	Buffer* m_pBuffer;		// 数据指针
	int m_cursor;			// 数据读取的偏移
};// end class Packet
/*--------------------------------------------------------------------*/
#define CLIENT_SELECT_TIMEOUT 30
#define PACKET_HEAD_LENGTH 8
#define IP_SIZE 18
class Client;
class ClientInterface
{
public:
	ClientInterface(void){}
	virtual ~ClientInterface(void){}

	virtual void notifyConnectServerFailed(Client* pClient) = 0;
	virtual void notifyConnectServerSuccess(Client* pClient) = 0;
	virtual void notifyConnectOut(Client* pClient) = 0;
	virtual void notifyPacketIn(Client* pClient, Packet* pPacket) = 0;
};// end class ClientInterface

enum ClientEventType{
	CLIENT_EVENT_NONE = 0,
	CLIENT_EVENT_CONN_FAILED = 1,
	CLIENT_EVENT_CONN_SUCCESS = 2,
	CLIENT_EVENT_CONN_OUT = 3,
	CLIENT_EVENT_PACKET_IN = 4,
};

typedef struct ClientEvent{
	ClientEventType event;
	Packet* pPacket;
}ClientEvent;

class Client : public RefObject, public Sync, public Thread
{
public:
	typedef std::deque<Packet*> PacketQueue;
	typedef std::deque<ClientEvent> ClientEventQueue;
public:
	Client(void);
	virtual ~Client(void);
	virtual int threadFunction(void);

	virtual void dispatchEvent(void);	// 这个函数需要在主循环中调用，用来分发事件

	virtual bool receivePacket(Packet* pPacket);
	virtual void removeSocket(void);
	virtual void setNotifyInterface(ClientInterface* pInterface){ m_pInterface = pInterface; }
	virtual inline void setSocket(const char* ip, unsigned short port){
		strcpy(m_ip, ip);
		m_port = port;
	}
	virtual inline void setSocketFD(int fd){ m_fd = fd; }
	virtual inline int getSocketFD(void) const { return m_fd; }
protected:
	virtual bool connectServer(void);
	virtual bool trySelectSocket(void);
	virtual bool tryReadSocket(void);
	virtual bool tryWriteSocket(void);
	virtual bool readSocket(void);
	virtual bool writePacket(void);
	virtual bool writeSocket(Packet* pPacket);
	virtual void closeSocket(void);
	virtual void addClientEvent(ClientEventType event, Packet* pPacket);
	void dispatchPacket(Packet* pPacket);
	void releasePacket(void);
protected:
    int m_fd;
    char m_ip[IP_SIZE];//192.168.110.110
    unsigned short m_port;
	PacketQueue m_packetQueue;
	ClientEventQueue m_clientEventQueue;
	ClientEventQueue m_tempEventQueue;
	Packet* m_tempReadPacket;
	ClientInterface* m_pInterface;
};//end class Client
/*--------------------------------------------------------------------*/

NS_HIVE_END

#endif
