//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/25
// Time: 下午11:12
// To change this template use File | Settings | File Templates.
//

#include "mainhandler.h"

NS_HIVE_BEGIN

MainHandler::MainHandler(void) : Handler(),
	m_destID(0), m_destPort(0), m_destEncrypt(false), m_destDecrypt(false),
	m_innerPort(0), m_innerEncrypt(false), m_innerDecrypt(false),
	m_socketPort(0), m_socketEncrypt(false), m_socketDecrypt(false),
	m_httpPort(0),
	m_httpsPort(0)
{

}
MainHandler::~MainHandler(void){

}
void MainHandler::onReceivePacket(Packet* pPacket, Task* pTask){
	LOG_DEBUG("packet length=%d", pPacket->getLength());
}
void MainHandler::onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK){
	LOG_DEBUG("buffer length=%d callbackID=%d isRequestOK=%d", pBuffer->size(), callbackID, isRequestOK);
}
void MainHandler::onOpenClientOK(uint32 clientHandle, OpenClientOKTask* pTask){
	LOG_DEBUG("clientHandle=%d", clientHandle);
}
void MainHandler::onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask){
	LOG_DEBUG("callbackID=%d clientHandle=%d", callbackID, clientHandle);
}
void MainHandler::onOpenSocketListener(uint32 callbackID, uint32 listenerHandle, OpenSocketListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);
}
void MainHandler::onOpenHttpListener(uint32 callbackID, uint32 listenerHandle, OpenHttpListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);
}
void MainHandler::onOpenHttpsListener(uint32 callbackID, uint32 listenerHandle, OpenHttpsListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);
}
void MainHandler::onCloseListener(uint32 callbackID, uint32 listenerHandle, CloseListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);
}
void MainHandler::onCloseConnect(uint32 callbackID, uint32 connectHandle, CloseConnectTask* pTask){
	LOG_DEBUG("callbackID=%d connectHandle=%d", callbackID, connectHandle);
}
int64 MainHandler::onTimerUpdate(uint32 callbackID){
	LOG_DEBUG("callbackID=%d", callbackID);
	return -1;
}

void MainHandler::onInitialize(void){
	LOG_DEBUG("main handler start...");
	openInnerListener();
	openSocketListener();
	openHttpListener();
	openHttpsListener();
	checkNodeConnect();
}
void MainHandler::checkNodeConnect(void){

}
void MainHandler::openInnerListener(void){

}
void MainHandler::openSocketListener(void){

}
void MainHandler::openHttpListener(void){

}
void MainHandler::openHttpsListener(void){

}

NS_HIVE_END










