//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/2/26
// Time: 下午8:52
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__epollworker__
#define __hive__epollworker__

#include "timer.h"
#include "epoll.h"
#include "destinationgroup.h"
#include "activeworker.h"
#include "accept.h"
#include "client.h"
#include "http.h"
#include "https.h"
#include "multicurl.h"
#include "globalhandler.h"

NS_HIVE_BEGIN

#define CONNECT_IDENTIFY_TIME 5000
#define CONNECT_ONLINE_TIME 15000
#define CONNECT_KEEP_ONLINE_TIME 5000
#define HTTP_HANDLE_TIMEOUT 30000

#define POOL_TYPE_ACCEPT 1
#define POOL_TYPE_CLIENT 2
#define POOL_TYPE_HTTP 3
#define POOL_TYPE_HTTPS 4

#define COMMAND_DISPATCH_BY_HANDLE 0
#define COMMAND_PING 255
#define COMMAND_PONG 254
#define COMMAND_REGISTER 253
#define COMMAND_RESPONSE 252
#define COMMAND_HIVE_REGISTER 251
#define COMMAND_HIVE_RESPONSE 250
#define COMMAND_DISPATCH_TRANSFER 249
#define COMMAND_RESPONSE_TRANSFER 248

#define EPOLL_READ_BUFFER_SIZE 8192

class MainWorker;

typedef DestinationGroup<EpollObject> EpollObjectDestinationGroup;

class EpollWorker : public ActiveWorker, public Thread
{
public:

public:
	char m_pReadBuffer[EPOLL_READ_BUFFER_SIZE];

	EpollObjectDestinationGroup* m_pGroup;
	MultiCurl* m_pMultiCurl;
	Packet* m_pPingPacket;				// 预先生成的ping数据包
	SSL_CTX* m_pSSLCTX;
	http_parser_settings m_settings;
public:
	EpollWorker(uint32 serviceID);
	virtual ~EpollWorker(void);

	static int64 checkConnectIdentify(Accept* pAccept);
	static int64 checkConnectOnline(Accept* pAccept);
	static int64 keepConnectOnline(Accept* pAccept);

	bool dispatchToConnect(uint32 handle, Packet* pPacket);

	uint32 openAccept(int fd, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex);
	uint32 openClient(uint32 bindHandle, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex);
	uint32 openHttp(int fd, const char* ip, uint16 port);
	uint32 openHttps(int fd, const char* ip, uint16 port);
	void receiveClient(Client* pClient);

	bool closeAccept(uint32 handle);
	bool closeClient(uint32 handle);
	bool closeHttp(uint32 handle);
	bool closeHttps(uint32 handle);
	bool closeConnect(uint32 handle);
	void notifyCloseConnect(Accept* pAccept);

	void sendCurlRequest(RequestData* pRequest){
		m_pMultiCurl->acceptRequestWithTask(pRequest);
	}
	// 有处理curl请求的机会
	void onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK);

	template<class _OBJECT_>
	_OBJECT_* getDestination(uint32 handle){
		return (_OBJECT_*)(m_pGroup->getDestination(handle));
	}
	inline EpollObjectDestinationGroup* getGroup(void) { return m_pGroup; }
	bool initHttpsCertificate(const char* publicKey, const char* privateKey);
	inline http_parser_settings* getSettings(void) { return &m_settings; }
	inline MultiCurl* getMultiCurl(void){ return m_pMultiCurl; }
	inline char* getReadBuffer(void){ return m_pReadBuffer; }
	inline Packet* getPingPacket(void){ return m_pPingPacket; }

	virtual int threadFunction(void);
	virtual void initialize(void);
	virtual void destroy(void);
};

class DispatchToConnectTask : public Task
{
public:
	Packet* m_pPacket;				// 请求request的packet
	uint32 m_handle;		// 请求的目标连接
public:
	DispatchToConnectTask(void) : Task(), m_pPacket(NULL) {}
	virtual ~DispatchToConnectTask(void){
		SAFE_RELEASE(m_pPacket)
	}

	virtual void doHandlerTask(Handler* pHandler){}
	virtual void doActiveTask(ActiveWorker* pHandler){
		EpollWorker* pWorker = (EpollWorker*)pHandler;
		pWorker->getGroup()->dispatchPacket(m_handle, m_pPacket, this);
	}
	void setPacket(Packet* pPacket){
		SAFE_RETAIN(pPacket)
		SAFE_RELEASE(m_pPacket)
		m_pPacket = pPacket;
	}
	void setDestinationHandle(uint32 handle){
		m_handle = handle;
	}
};

class OpenAcceptTask : public Task
{
public:
	int m_fd;
    char m_ip[IP_SIZE];//192.168.110.110
    uint16 m_port;
    bool m_isNeedEncrypt;
    bool m_isNeedDecrypt;
    uint8 m_acceptIndex;
public:
	OpenAcceptTask(void) : Task() {}
	virtual ~OpenAcceptTask(void){}

	virtual void doHandlerTask(Handler* pHandler){}
	virtual void doActiveTask(ActiveWorker* pHandler){
		EpollWorker* pWorker = (EpollWorker*)pHandler;
		pWorker->openAccept(m_fd, m_ip, m_port, m_isNeedEncrypt, m_isNeedDecrypt, m_acceptIndex);
	}
	inline void setSocket(const char* ip, uint16 port){
		strcpy(m_ip, ip);
		m_port = port;
	}
};

class BindAcceptHandleTask : public Task
{
public:
	uint32 m_bindHandle;
	uint32 m_acceptHandle;
public:
	BindAcceptHandleTask(void) : Task() {}
	virtual ~BindAcceptHandleTask(void){}

	virtual void doHandlerTask(Handler* pHandler){}
	virtual void doActiveTask(ActiveWorker* pHandler){
		EpollWorker* pWorker = (EpollWorker*)pHandler;
		Accept* pAccept = pWorker->getDestination<Accept>(m_acceptHandle);
		if(NULL != pAccept){
			pAccept->setBindHandle(m_bindHandle);
		}
	}
};

class OpenClientTask : public Task
{
public:
	uint32 m_callbackID;
	uint32 m_bindHandle;
    char m_ip[IP_SIZE];//192.168.110.110
    uint16 m_port;
    bool m_isNeedEncrypt;
    bool m_isNeedDecrypt;
    uint8 m_acceptIndex;
public:
	OpenClientTask(void) : Task() {}
	virtual ~OpenClientTask(void){}

	virtual void doHandlerTask(Handler* pHandler){
		pHandler->onOpenClient(m_callbackID, m_bindHandle, this);
	}
	virtual void doActiveTask(ActiveWorker* pHandler){
		EpollWorker* pWorker = (EpollWorker*)pHandler;
		uint32 handle = pWorker->openClient(m_bindHandle, m_ip, m_port, m_isNeedEncrypt, m_isNeedDecrypt, m_acceptIndex);
		uint32 bindHandle = m_bindHandle;
		this->m_bindHandle = handle;
		GlobalHandler::getInstance()->dispatchTask(bindHandle, this);
	}
	inline void setSocket(const char* ip, uint16 port){
		strcpy(m_ip, ip);
		m_port = port;
	}
};
class OpenClientOKTask : public Task
{
public:
	uint32 clientHandle;
public:
	OpenClientOKTask(void) : Task() {}
	virtual ~OpenClientOKTask(void){}
	virtual void doHandlerTask(Handler* pHandler){
		pHandler->onOpenClientOK(clientHandle, this);
	}
	virtual void doActiveTask(ActiveWorker* pHandler){}
};
class CloseConnectTask : public Task
{
public:
	uint32 m_callbackID;
	uint32 m_bindHandle;
	uint32 m_connectHandle;
public:
	CloseConnectTask(void) : Task(), m_callbackID(0), m_bindHandle(0), m_connectHandle(0) {}
	virtual ~CloseConnectTask(void){}
	virtual void doHandlerTask(Handler* pHandler){
		pHandler->onCloseConnect(m_callbackID, m_connectHandle, this);
	}
	virtual void doActiveTask(ActiveWorker* pHandler){
		EpollWorker* pWorker = (EpollWorker*)pHandler;
		if( !pWorker->closeConnect(m_connectHandle) ){
			LOG_ERROR("CloseConnectTask Connect not found handle=%d", m_connectHandle);
		}
		GlobalHandler::getInstance()->dispatchTask(m_bindHandle, this);
	}
};

class OpenHttpTask : public Task
{
public:
	int m_fd;
    char m_ip[IP_SIZE];//192.168.110.110
    uint16 m_port;
public:
	OpenHttpTask(void) : Task() {}
	virtual ~OpenHttpTask(void){}

	virtual void doHandlerTask(Handler* pHandler){}
	virtual void doActiveTask(ActiveWorker* pHandler){
		EpollWorker* pWorker = (EpollWorker*)pHandler;
		pWorker->openHttp(m_fd, m_ip, m_port);
	}
	inline void setSocket(const char* ip, uint16 port){
		strcpy(m_ip, ip);
		m_port = port;
	}
};
class OpenHttpsTask : public Task
{
public:
	int m_fd;
    char m_ip[IP_SIZE];//192.168.110.110
    uint16 m_port;
public:
	OpenHttpsTask(void) : Task() {}
	virtual ~OpenHttpsTask(void){}

	virtual void doHandlerTask(Handler* pHandler){}
	virtual void doActiveTask(ActiveWorker* pHandler){
		EpollWorker* pWorker = (EpollWorker*)pHandler;
		pWorker->openHttps(m_fd, m_ip, m_port);
	}
	inline void setSocket(const char* ip, uint16 port){
		strcpy(m_ip, ip);
		m_port = port;
	}
};

NS_HIVE_END

#endif
