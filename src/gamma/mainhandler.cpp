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
	memset(m_hiveNodes, 0, sizeof(m_hiveNodes));
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
	LOG_DEBUG("client open success clientHandle=%d", clientHandle);

}
void MainHandler::onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask){
	LOG_DEBUG("client open processing callbackID=%d clientHandle=%d", callbackID, clientHandle);
	m_handleToNode[clientHandle] = callbackID;
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
	// check if callbackID > 0 , then connect again in seconds

}
int64 MainHandler::onTimerUpdate(uint32 callbackID){
	LOG_DEBUG("callbackID=%d", callbackID);
	return -1;
}

bool MainHandler::registerNode(const char* ptr){
	HiveInformation regInfo;
	regInfo.set(ptr);
	return registerNode(regInfo);
}
bool MainHandler::registerNode(uint32 id, const char* ip, uint16 port, bool encrypt, bool decrypt){
	HiveInformation regInfo;
	regInfo.set(id, ip, port, encrypt, decrypt);
	return registerNode(regInfo);
}
bool MainHandler::registerNode(const HiveInformation& regInfo){
	if(regInfo.id > MAX_NODE_NUMBER){
		LOG_ERROR("you are trying to register a node id > MAX_NODE_NUMBER id=%d", regInfo.id);
        return false;
	}
	HiveInformation& info = m_hiveNodes[regInfo.id];
	if(info == regInfo){
		LOG_INFO("current node info equal to register node");
		return true;
	}
	unregisterNode(regInfo.id);
	info.set(regInfo.get());
	checkNodeConnect(info.id);
	return true;
}
bool MainHandler::unregisterNode(uint32 id){
	if(id > MAX_NODE_NUMBER){
		LOG_ERROR("you are trying to unregister a node id > MAX_NODE_NUMBER id=%d", id);
		return false;
	}
	HiveInformation& info = m_hiveNodes[id];
	if(info.id > 0){
		info.reset();                           // reset the data
		uint32 connectHandle = GlobalService::getInstance()->getNodeConnect(id);
		if(connectHandle > 0){
			GlobalService::getInstance()->removeNodeConnect(id);    // remove the node connection
			closeConnect(0, connectHandle);    // close connection
			return true;
		}
	}
	return false;
}
void MainHandler::onInitialize(void){
	LOG_DEBUG("main handler start...");
	openInnerListener();
	openMainSocketListener();
	openMainHttpListener();
	openMainHttpsListener();
	// register self
	uint32 nodeID = GlobalSetting::getInstance()->getNodeID();
	registerNode(nodeID, m_innerIP.c_str(), m_innerPort, m_innerEncrypt, m_innerDecrypt);
	// register discovery node
	registerNode(m_destID, m_destIP.c_str(), m_destPort, m_destEncrypt, m_destDecrypt);
}
void MainHandler::checkNodeConnect(uint32 id){
	// try to connect the destination node
	uint32 nodeID = GlobalSetting::getInstance()->getNodeID();
	if( id == nodeID ){
		LOG_DEBUG("the current node is the destination node");
	}else{
		// if the connection is already connected, skip
		uint32 connHandle = GlobalService::getInstance()->getNodeConnect(id);
		if(connHandle > 0){
			// do nothing
		}else{
			HiveInformation& info = m_hiveNodes[id];
			LOG_DEBUG("try to open client to node=%d ip=%s port=%d", id, info.ip, info.port);
			openClient(id, info.ip, info.port, info.encrypt, info.decrypt, DEFAULT_ACCEPT_INDEX);
		}
	}
}
void MainHandler::openInnerListener(void){
	LOG_DEBUG("ip=%s port=%d", m_innerIP.c_str(), m_innerPort);
	this->openSocketListener(0, m_innerIP.c_str(), m_innerPort, m_innerEncrypt, m_innerDecrypt, 0);
}
void MainHandler::openMainSocketListener(void){
	LOG_DEBUG("ip=%s port=%d", m_socketIP.c_str(), m_socketPort);
	this->openSocketListener(0, m_socketIP.c_str(), m_socketPort, m_socketEncrypt, m_socketDecrypt, 0);
}
void MainHandler::openMainHttpListener(void){
	LOG_DEBUG("ip=%s port=%d", m_httpIP.c_str(), m_httpPort);
	this->openHttpListener(0, m_httpIP.c_str(), m_httpPort);
}
void MainHandler::openMainHttpsListener(void){
	LOG_DEBUG("ip=%s port=%d", m_httpsIP.c_str(), m_httpsPort);
	this->openHttpsListener(0, m_httpsIP.c_str(), m_httpsPort);
}

NS_HIVE_END










