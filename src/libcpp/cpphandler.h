//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/13
// Time: 下午9:13
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__cpphandler__
#define __hive__cpphandler__

#include "core.h"

NS_HIVE_BEGIN

class CppHandler : public Handler
{
public:

public:
	CppHandler(void);
	virtual ~CppHandler(void);

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
	virtual void onInitialize(void);
	virtual void onDestroy(void);
	virtual int64 onTimerUpdate(uint32 callbackID);



};

NS_HIVE_END

#endif
