//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/2/26
// Time: 下午8:53
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__mainworker__
#define __hive__mainworker__

#include "epoll.h"
#include "timer.h"
#include "epollworker.h"
#include "task.h"
#include "handler.h"
#include "handlerqueue.h"
#include "worker.h"
#include "globalhandler.h"
#include "globalservice.h"
#include "timerhandle.h"

NS_HIVE_BEGIN

#define NET_KEY_LENGTH 16

class MainWorker : public ActiveWorker
{
public:
	DestinationPool* m_pListenerPool;
	uint16 m_nodeID;
	char m_key[NET_KEY_LENGTH];					// 密钥
public:
	MainWorker(void);
	virtual ~MainWorker(void);

	static MainWorker* getInstance(void);
	static MainWorker* createInstance(void);
	static void destroyInstance(void);

	static void onAcceptSocket(int fd, const char* ip, uint16 port, Listener* pListener);
	static void onAcceptHttp(int fd, const char* ip, uint16 port, Listener* pListener);
	static void onAcceptHttps(int fd, const char* ip, uint16 port, Listener* pListener);

	bool closeListener(uint32 handle);
	uint32 openListener(const char* ip, uint16 port, AcceptSocketFunction pFunc, bool isNeedEncrypt, bool isNeedDecrypt);
	Listener* getListener(uint32 handle){
		return (Listener*)m_pListenerPool->get(handle);
	}
	uint32 openHttpListener(const char* ip, uint16 port);
	uint32 openHttpsListener(const char* ip, uint16 port);
	uint32 openSocketListener(const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt);

	bool dispatchToMain(Packet* pPacket);
	void onReceivePacket(Packet* pPacket, Task* pTask);

	void update(void);

	virtual void initialize(uint16 nodeID, uint16 epollWorkerNumber, uint16 workerNumber);
	virtual void destroy(void);

	inline bool setKey(const char* key){
		if(strlen(key) < NET_KEY_LENGTH){
			return false;
		}
		memcpy(m_key, key, NET_KEY_LENGTH);
		return true;
	}
	inline const char* getKey(void) const { return m_key; }
	inline uint16 getNodeID(void){ return m_nodeID; }
	inline void setNodeID(uint16 nodeID){ m_nodeID = nodeID; }
};

class DispatchToMainTask : public Task
{
public:
	Packet* m_pPacket;		// 请求request的packet
public:
	DispatchToMainTask(void) : Task(), m_pPacket(NULL) {}
	virtual ~DispatchToMainTask(void){
		SAFE_RELEASE(m_pPacket)
	}

	virtual void doTask(Handler* pHandler){}
	virtual void doTask(ActiveWorker* pHandler){
		MainWorker* pWorker = (MainWorker*)pHandler;
		pWorker->onReceivePacket(m_pPacket, this);
	}
	void setPacket(Packet* pPacket){
		SAFE_RETAIN(pPacket)
		SAFE_RELEASE(m_pPacket)
		m_pPacket = pPacket;
	}
};

class OpenSocketListenerTask : public Task
{
public:
	uint32 m_callbackID;
	uint32 m_bindHandle;
    char m_ip[IP_SIZE];//192.168.110.110
    uint16 m_port;
    bool m_isNeedEncrypt;
    bool m_isNeedDecrypt;
public:
	OpenSocketListenerTask(void) : Task() {}
	virtual ~OpenSocketListenerTask(void){}

	virtual void doTask(Handler* pHandler){
		pHandler->onOpenSocketListener(m_callbackID, m_bindHandle, this);
	}
	virtual void doTask(ActiveWorker* pHandler){
		MainWorker* pWorker = (MainWorker*)pHandler;
		uint32 handle = pWorker->openSocketListener(m_ip, m_port, m_isNeedEncrypt, m_isNeedDecrypt);
		// 返回创建的结果
		uint32 bindHandle = m_bindHandle;
		m_bindHandle = handle;
		GlobalHandler::getInstance()->dispatchTask(bindHandle, this);
	}
	inline void setSocket(const char* ip, uint16 port){
		strcpy(m_ip, ip);
		m_port = port;
	}
};

class OpenHttpListenerTask : public Task
{
public:
	uint32 m_callbackID;
	uint32 m_bindHandle;
    char m_ip[IP_SIZE];//192.168.110.110
    uint16 m_port;
public:
	OpenHttpListenerTask(void) : Task() {}
	virtual ~OpenHttpListenerTask(void){}

	virtual void doTask(Handler* pHandler){
		pHandler->onOpenHttpListener(m_callbackID, m_bindHandle, this);
	}
	virtual void doTask(ActiveWorker* pHandler){
		MainWorker* pWorker = (MainWorker*)pHandler;
		uint32 handle = pWorker->openHttpListener(m_ip, m_port);
		// 返回创建的结果
		uint32 bindHandle = m_bindHandle;
		m_bindHandle = handle;
		GlobalHandler::getInstance()->dispatchTask(bindHandle, this);
	}
	inline void setSocket(const char* ip, uint16 port){
		strcpy(m_ip, ip);
		m_port = port;
	}
};

class OpenHttpsListenerTask : public Task
{
public:
	uint32 m_callbackID;
	uint32 m_bindHandle;
    char m_ip[IP_SIZE];//192.168.110.110
    uint16 m_port;
public:
	OpenHttpsListenerTask(void) : Task() {}
	virtual ~OpenHttpsListenerTask(void){}

	virtual void doTask(Handler* pHandler){
		pHandler->onOpenHttpsListener(m_callbackID, m_bindHandle, this);
	}
	virtual void doTask(ActiveWorker* pHandler){
		MainWorker* pWorker = (MainWorker*)pHandler;
		uint32 handle = pWorker->openHttpsListener(m_ip, m_port);
		// 返回创建的结果
		uint32 bindHandle = m_bindHandle;
		m_bindHandle = handle;
		GlobalHandler::getInstance()->dispatchTask(bindHandle, this);
	}
	inline void setSocket(const char* ip, uint16 port){
		strcpy(m_ip, ip);
		m_port = port;
	}
};

class CloseListenerTask : public Task
{
public:
	uint32 m_callbackID;
	uint32 m_bindHandle;
	uint32 m_listenerHandle;
public:
	CloseListenerTask(void) : Task() {}
	virtual ~CloseListenerTask(void){}

	virtual void doTask(Handler* pHandler){
		pHandler->onCloseListener(m_callbackID, m_bindHandle, this);
	}
	virtual void doTask(ActiveWorker* pHandler){
		MainWorker* pWorker = (MainWorker*)pHandler;
		if( !pWorker->closeListener(m_listenerHandle) ){
			LOG_ERROR("Listener not found handle=%d", m_listenerHandle);
		}
		GlobalHandler::getInstance()->dispatchTask(m_bindHandle, this);
	}
};

NS_HIVE_END

#endif
