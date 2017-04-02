//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/31
// Time: 下午9:38
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__accept__
#define __hive__accept__

#include "epoll.h"
#include "object.h"
#include "packet.h"
#include "util.h"
#include "timer.h"
#include "destination.h"

NS_HIVE_BEGIN

#define MAX_LENGTH_NOT_IDENTIFY 128
#define MAX_LENGTH_IDENTIFY 4194312	// 4M + 8

enum ConnectionState {
	CS_DISCONNECT = 0,
	CS_CONNECT_START,
	CS_CONNECT_OK,
	CS_IDENTIFY_ING,
	CS_IDENTIFY_OK,
};

class Accept;
class EpollWorker;

typedef int64 (*ConnectTimeoutCallback)(Accept* pAccept);

class Accept : public EpollConnectObject, public Destination, public TimerObject
{
public:
	typedef std::deque<Packet*> PacketQueue;
protected:
	ConnectTimeoutCallback m_timerCallback;	// 回调函数
	PacketQueue m_packetQueue;
	EpollWorker* m_pEpollWorker;
	Packet* m_tempReadPacket;
	uint32 m_bindHandle;					// 绑定的句柄，主要用于离线的时候，通知对方连接离线
	unsigned char m_connectionState;
	bool m_isOnline;
	bool m_isNeedEncrypt;			// 是否需要解密
	bool m_isNeedDecrypt;			// 是否需要加密
	uint8 m_acceptIndex;			// 解析接收数据的方法下标
public:
	explicit Accept(void);
	virtual ~Accept(void);

	static Destination* createObject(void){
		return new Accept();
	}
	static void releaseObject(Destination* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	// from EpollObject
	virtual bool epollActive(uint32 events){ return false; }
	virtual void epollIn(void);
	virtual void epollOut(void);
	virtual void epollRemove(void);
	virtual void epollCheck(void);

	// from Destination
	virtual void onReceivePacket(Packet* pPacket, Task* pTask);

	// from TimerObject
	virtual int64 timerCallback(void);

	void dispatchPacket(Packet* pPacket, uint8 command);

	bool setTimeout(int64 timeCount, Timer* pTimer, ConnectTimeoutCallback callback);
	bool sendPacket(Packet* pPacket);
	bool sendPacket(const char* ptr, int length);
	inline bool isOnline(void) const { return m_isOnline; }
	inline void setOnline(bool online) { m_isOnline = online; }
	inline void setConnectionState(unsigned char state) { m_connectionState = state; }
	inline unsigned char getConnectionState(void) const { return (unsigned char)m_connectionState; }
	inline bool isIdentify(void) const { return (unsigned char)m_connectionState >= CS_IDENTIFY_OK; }
	inline int getMaxLength(void) const {
		if(isIdentify()){
			return MAX_LENGTH_IDENTIFY;
		}else{
			return MAX_LENGTH_NOT_IDENTIFY;
		}
	}
	void resetData(void);

	inline void setTempReadPacket(Packet* pPacket){ m_tempReadPacket = pPacket; }
	inline Packet* getTempReadPacket(void){ return m_tempReadPacket; }
	inline void setBindHandle(uint32 handle){ m_bindHandle = handle; }
	inline uint32 getBindHandle(void) const { return m_bindHandle; }
	inline bool isNeedEncrypt(void) const { return m_isNeedEncrypt; }
	inline void setIsNeedEncrypt(bool need) { m_isNeedEncrypt = need; }
	inline bool isNeedDecrypt(void) const { return m_isNeedDecrypt; }
	inline void setIsNeedDecrypt(bool need) { m_isNeedDecrypt = need; }
	inline uint8 getAcceptIndex(void) const { return m_acceptIndex; }
	inline void setAcceptIndex(uint8 index){ m_acceptIndex = index; }
	inline void setEpollWorker(EpollWorker* pEpollWorker){ m_pEpollWorker = pEpollWorker; }
	inline EpollWorker* getEpollWorker(void){ return m_pEpollWorker; }
protected:
	int readSocket(void);
	int writeSocket(Packet* pPacket);
	int writeSocket(const char* ptr, int length, int* writeLength);
	void releasePacket(void);
};//end class Accept

NS_HIVE_END

#endif
