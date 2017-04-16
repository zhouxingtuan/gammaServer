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
//	m_handleToNode[clientHandle] = callbackID;
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
	// try to connect the destination node
	uint32 nodeID = GlobalSetting::getInstance()->getNodeID();
	if( m_destID == nodeID ){
		LOG_DEBUG("the current node is the destination node");
	}else{
		// if the connection is already connected, skip
		uint32 connHandle = GlobalService::getInstance()->getNodeConnect(m_destID);
		if(connHandle > 0){
			// do nothing
		}else{
			this->openClient(m_destID, m_destIP.c_str(), m_destPort,
        		m_destEncrypt, m_destDecrypt, 0);
		}
	}
}
void MainHandler::openInnerListener(void){
	this->openSocketListener(0, m_innerIP.c_str(), m_innerPort,
		m_innerEncrypt, m_innerDecrypt, 0);
}
void MainHandler::openSocketListener(void){
	this->openSocketListener(0, m_socketIP.c_str(), m_socketPort,
		m_socketEncrypt, m_socketDecrypt, 0);
}
void MainHandler::openHttpListener(void){
	this->openHttpListener(0, m_httpIP.c_str(), m_httpPort);
}
void MainHandler::openHttpsListener(void){
	this->openHttpsListener(0, m_httpsIP.c_str(), m_httpsPort);
}

NS_HIVE_END










