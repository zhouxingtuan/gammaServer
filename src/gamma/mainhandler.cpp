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
	// handle response register hive here
	uint32 command = pPacket->getCommand();
	switch(command){
		case COMMAND_RESPONSE:{
			uint32 connectHandle = pPacket->getDestination();
			registerHive(connectHandle);
			break;
		}
		case COMMAND_HIVE_REGISTER:{
			uint32 connectHandle = pPacket->getDestination();
			LOG_DEBUG("COMMAND_HIVE_REGISTER get connectHandle=%d", connectHandle);
			pPacket->readBegin();
			// get all the node info and record
			recordHiveInfo(pPacket);
			recordModuleInfo(pPacket);
			pPacket->readEnd();
			// response the remote node, current node info
			pPacket->writeBegin(COMMAND_HIVE_RESPONSE, connectHandle);
			writeHiveInfo(pPacket);
			writeModuleInfo(pPacket);
			pPacket->writeEnd();
			bool result = GlobalService::getInstance()->sendToService(connectHandle, pPacket);
			LOG_DEBUG("COMMAND_HIVE_RESPONSE to connectHandle=%d command=%d result=%d", connectHandle, pPacket->getCommand(), result);
			break;
		}
		case COMMAND_HIVE_RESPONSE:{
			uint32 connectHandle = pPacket->getDestination();
			LOG_DEBUG("COMMAND_HIVE_RESPONSE get connectHandle=%d", connectHandle);
			// record all the node info
			recordHiveInfo(pPacket);
			recordModuleInfo(pPacket);
			break;
		}
		default:{
			LOG_ERROR("command for MainHandler is not handle");
			break;
		}
	};
}
void MainHandler::onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK){
	LOG_DEBUG("buffer length=%d callbackID=%d isRequestOK=%d", pBuffer->size(), callbackID, isRequestOK);

}
void MainHandler::onOpenClientOK(uint32 clientHandle, OpenClientOKTask* pTask){
	LOG_DEBUG("client open success clientHandle=%d", clientHandle);
	// check and set the node connection
	HandleToNodeMap::iterator itCur = m_handleToNode.find(clientHandle);
	if(itCur != m_handleToNode.end()){
		uint32 id = itCur->second;
		GlobalNode::getInstance()->setNodeConnect(id, clientHandle);
	}
	// register the client to remote node
	identifyHive(clientHandle);
}
void MainHandler::onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask){
	LOG_DEBUG("client open processing callbackID=%d clientHandle=%d", callbackID, clientHandle);
	// record the client handle to node id
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
	// check handle connection out
	HandleToNodeMap::iterator itCur = m_handleToNode.find(connectHandle);
    if(itCur != m_handleToNode.end()){
        uint32 id = itCur->second;
        m_handleToNode.erase(itCur);
        GlobalNode::getInstance()->removeNodeConnect(id);
        // connect again in seconds
        LOG_DEBUG("schedule node to reconnect id=%d", id);
        startTimer(id, NODE_RECONNECT_TIME);
    }
}
void MainHandler::onInitialize(const std::string& param){
	LOG_DEBUG("onInitialize called handle=%d param=%s", getHandle(), param.c_str());
}
void MainHandler::onDestroy(void){
	LOG_DEBUG("onDestroy called handle=%d", getHandle());
}
int64 MainHandler::onTimerUpdate(uint32 callbackID){
	LOG_DEBUG("callbackID=%d", callbackID);
	// try to connect the node again
	checkNodeConnect(callbackID);
	return -1;
}

void MainHandler::recordHiveInfo(Packet* pPacket){
	int32 arraySize = 0;
	pPacket->read(&arraySize, sizeof(int32));
	for(auto i=0; i<arraySize; ++i){
		HiveInformation info;
		pPacket->read(&info, sizeof(HiveInformation));
		LOG_DEBUG("registerHive with id=%d ip=%s port=%d encrypt=%d decrypt=%d", info.id, info.ip, info.port, info.encrypt, info.decrypt);
		registerNode(info);
	}
}
void MainHandler::writeHiveInfo(Packet* pPacket){
	int arrayOffset = pPacket->beginArray<int32>();
	for(auto &info : m_hiveNodes){
		if(info.id > 0){
			LOG_DEBUG("registerHive with id=%d ip=%s port=%d encrypt=%d decrypt=%d", info.id, info.ip, info.port, info.encrypt, info.decrypt);
			pPacket->writeArray<int32>(info.get(), sizeof(HiveInformation), arrayOffset);
		}
	}
}
void MainHandler::writeModuleInfo(Packet* pPacket){
	ModuleTypeVector modules;
	GlobalModule::getInstance()->copyModules(modules);
	int moduleSize = (int)modules.size();
	pPacket->write(&moduleSize, sizeof(int));
	for( auto &group : modules ){
		ModuleHandlerInformationVector& infoVec = group.infoVec;
		int arrayOffset = pPacket->beginArray<int32>();
		for( auto &info : infoVec ){
			pPacket->writeArray<int32>(&info, sizeof(ModuleHandlerInformation), arrayOffset);
		}
	}
}
void MainHandler::recordModuleInfo(Packet* pPacket){
	int moduleSize = 0;
	pPacket->read(&moduleSize, sizeof(int32));
	for(int i=0; i<moduleSize; ++i){
		int arraySize = 0;
		pPacket->read(&arraySize, sizeof(int32));
		for(int j=0; j<arraySize; ++j){
			ModuleHandlerInformation info;
			pPacket->read(&info, sizeof(ModuleHandlerInformation));
			if(info.handle > 0){
				bool result = GlobalModule::getInstance()->addInformation(info.moduleType, info.moduleIndex, info.handle, info.version);
                LOG_DEBUG("addInformation moduleType=%d moduleIndex=%d handle=%d version=%d result=%d",
                    info.moduleType, info.moduleIndex, info.handle, info.version,result);
			}else{
				LOG_DEBUG("info.handle <= 0 skip");
			}
		}
	}
}
void MainHandler::registerHive(uint32 connectHandle){
	LOG_DEBUG("start register hive connectHandle=%d", connectHandle);
	Packet* pPacket = new Packet(1024);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_HIVE_REGISTER, connectHandle);
	writeHiveInfo(pPacket);
	writeModuleInfo(pPacket);
	pPacket->writeEnd();
	bool result = GlobalService::getInstance()->sendToService(connectHandle, pPacket);
	LOG_DEBUG("registerHive to connectHandle=%d command=%d result=%d", connectHandle, pPacket->getCommand(), result);
	pPacket->release();
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
	LOG_DEBUG("registerNode id=%d ip=%s port=%d encrypt=%d decrypt=%d", regInfo.id, regInfo.ip, regInfo.port, regInfo.encrypt, regInfo.decrypt);
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
		uint32 connectHandle = GlobalNode::getInstance()->getNodeConnect(id);
		if(connectHandle > 0){
			GlobalNode::getInstance()->removeNodeConnect(id);    // remove the node connection
			closeConnect(0, connectHandle);    // close connection
			return true;
		}
	}
	return false;
}
void MainHandler::onConnectInitialize(void){
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
		uint32 connectHandle = GlobalNode::getInstance()->getNodeConnect(id);
		if(connectHandle > 0){
			// do nothing
			LOG_DEBUG("the current node is already connected id=%d connectHandle=%d", id, connectHandle);
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










