//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/25
// Time: 下午11:12
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__mainhandler__
#define __hive__mainhandler__

#include "core.h"

NS_HIVE_BEGIN

#define HANDLER_TYPE_MAIN 0
#define MAIN_HANDLER_INDEX 1

#define DEFAULT_ACCEPT_INDEX 0

typedef struct HiveInformation{
    uint32 id;
    char ip[IP_SIZE];//192.168.110.110
    uint16 port;
    bool encrypt;
    bool decrypt;
    HiveInformation(void){
        reset();
    }
    ~HiveInformation(void){}
    void set(const char* ptr){
        memcpy((char*)get(), ptr, sizeof(HiveInformation));
    }
    const char* get(void) const {
        return ((char*)(&id));
    }
    void set(uint32 id, const char* ip, uint16 port, bool encrypt, bool decrypt){
        this->id = id;
        memcpy(this->ip, ip, IP_SIZE);
        this->port = port;
        this->encrypt = encrypt;
        this->decrypt = decrypt;
    }
    void reset(void){
        memset(&id, sizeof(HiveInformation), 0);
    }
	inline bool operator==(const HiveInformation& h) const{
		return (memcmp(get(), (h.get()), sizeof(HiveInformation)) == 0);
	}
	inline HiveInformation& operator=(const HiveInformation& h){
		set(h.get());
		return *this;
	}
}HiveInformation;

class MainHandler : public Handler
{
public:
	typedef std::map<uint32, uint32> HandleToNodeMap;
	typedef std::vector<uint32> UIntVector;
public:
	// discovery node information
	uint32 m_destID;
	std::string m_destIP;
	uint16 m_destPort;
	bool m_destEncrypt;
	bool m_destDecrypt;

	// inner address
	std::string m_innerIP;
	uint16 m_innerPort;
	bool m_innerEncrypt;
	bool m_innerDecrypt;

	// outer address
	std::string m_socketIP;
	uint16 m_socketPort;
	bool m_socketEncrypt;
	bool m_socketDecrypt;

	// http address
	std::string m_httpIP;
	uint16 m_httpPort;

	// https address
	std::string m_httpsIP;
	uint16 m_httpsPort;

	// connectHandle->nodeID
	HandleToNodeMap m_handleToNode;
	HiveInformation m_hiveNodes[MAX_NODE_NUMBER + 1];
	UIntVector m_waitNodes; // wait in queue to check connect
public:
	MainHandler(void);
	virtual ~MainHandler(void);

	// from Destination
	virtual void onReceivePacket(Packet* pPacket, Task* pTask);

	// from Handler
	virtual void onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK);
	virtual void onOpenClientOK(uint32 clientHandle, OpenClientOKTask* pTask);
	virtual void onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask);
	virtual void onOpenSocketListener(uint32 callbackID, uint32 listenerHandle, OpenSocketListenerTask* pTask);
	virtual void onOpenHttpListener(uint32 callbackID, uint32 listenerHandle, OpenHttpListenerTask* pTask);
	virtual void onOpenHttpsListener(uint32 callbackID, uint32 listenerHandle, OpenHttpsListenerTask* pTask);
	virtual void onCloseListener(uint32 callbackID, uint32 listenerHandle, CloseListenerTask* pTask);
	virtual void onCloseConnect(uint32 callbackID, uint32 connectHandle, CloseConnectTask* pTask);
	virtual int64 onTimerUpdate(uint32 callbackID);

	void onInitialize(void);

	bool registerNode(const char* ptr);
	bool registerNode(uint32 id, const char* ip, uint16 port, bool encrypt, bool decrypt);
	bool registerNode(const HiveInformation& regInfo);
	bool unregisterNode(uint32 id);
protected:
	void checkNodeConnect(uint32 id);
	void openInnerListener(void);
	void openMainSocketListener(void);
	void openMainHttpListener(void);
	void openMainHttpsListener(void);
};

class StartMainHandlerTask : public Task
{
public:
	StartMainHandlerTask(void) : Task(){}
	virtual ~StartMainHandlerTask(void){}

	virtual void doHandlerTask(Handler* pHandler){
		MainHandler* pMain = (MainHandler*)pHandler;
		pMain->onInitialize();
	}
	virtual void doActiveTask(ActiveWorker* pHandler){}
};

NS_HIVE_END

#endif
