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

class MainHandler : public Handler
{
public:
	MainHandler(void);
	virtual ~MainHandler(void);

	static Destination* createObject(void){
		return new MainHandler();
	}
	static void releaseObject(Destination* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

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
};

class StartMainHandlerTask : public Task
{
public:
	StartMainHandlerTask(void) : Task(){}
	virtual ~StartMainHandlerTask(void){}

	virtual void doTask(Handler* pHandler){
		MainHandler* pMain = (MainHandler*)pHandler;
		pHandler->onInitialize();
	}
	virtual void doTask(ActiveWorker* pHandler){}
};

NS_HIVE_END

#endif
