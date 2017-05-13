//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/13
// Time: 下午9:14
// To change this template use File | Settings | File Templates.
//

#include "cpphandler.h"

NS_HIVE_BEGIN

CppHandler::CppHandler(void) : Handler(){

}
CppHandler::~CppHandler(void){

}

void CppHandler::onReceivePacket(Packet* pPacket, Task* pTask){
	LOG_DEBUG("packet length=%d", pPacket->getLength());

}
void CppHandler::onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK){
	LOG_DEBUG("buffer length=%d callbackID=%d isRequestOK=%d", pBuffer->size(), callbackID, isRequestOK);

}
void CppHandler::onOpenClientOK(uint32 clientHandle, OpenClientOKTask* pTask){
	LOG_DEBUG("client open success clientHandle=%d", clientHandle);

}
void CppHandler::onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask){
	LOG_DEBUG("client open processing callbackID=%d clientHandle=%d", callbackID, clientHandle);

}
void CppHandler::onOpenSocketListener(uint32 callbackID, uint32 listenerHandle, OpenSocketListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);

}
void CppHandler::onOpenHttpListener(uint32 callbackID, uint32 listenerHandle, OpenHttpListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);

}
void CppHandler::onOpenHttpsListener(uint32 callbackID, uint32 listenerHandle, OpenHttpsListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);

}
void CppHandler::onCloseListener(uint32 callbackID, uint32 listenerHandle, CloseListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);

}
void CppHandler::onCloseConnect(uint32 callbackID, uint32 connectHandle, CloseConnectTask* pTask){
	LOG_DEBUG("callbackID=%d connectHandle=%d", callbackID, connectHandle);

}
void CppHandler::onInitialize(void){
	LOG_DEBUG("onInitialize called handle=%d", getHandle());
}
void CppHandler::onDestroy(void){
	LOG_DEBUG("onDestroy called handle=%d", getHandle());
}
int64 CppHandler::onTimerUpdate(uint32 callbackID){
	LOG_DEBUG("callbackID=%d", callbackID);

	return -1;
}

NS_HIVE_END
