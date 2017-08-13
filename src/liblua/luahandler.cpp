//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/13
// Time: 下午9:14
// To change this template use File | Settings | File Templates.
//

#include "luahandler.h"

NS_HIVE_BEGIN

LuaHandler::LuaHandler(void) : Handler(), m_pScript(NULL), m_pDBRedisPool(NULL), m_pDBMysqlPool(NULL), m_pMD5Pool(NULL)
{

}
LuaHandler::~LuaHandler(void){
	destroyHandler();
}

void LuaHandler::onReceivePacket(Packet* pPacket, Task* pTask){
	uint32 command = pPacket->getCommand();
	uint32 destination = pPacket->getDestination();
	LOG_DEBUG("packet length=%d command=%d destination=%d", pPacket->getLength(), command, destination);
	m_pScript->callFunctionNNR("onReceivePacket", command, destination, pPacket->getBody(), pPacket->getBodyLength(), 0);
}
void LuaHandler::onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK){
	LOG_DEBUG("buffer length=%d callbackID=%d isRequestOK=%d", pBuffer->size(), callbackID, isRequestOK);
	m_pScript->callFunctionBNR("onCurlResponse", isRequestOK, callbackID, pBuffer->data(), pBuffer->size(), 0);
}
void LuaHandler::onOpenClientOK(uint32 clientHandle, OpenClientOKTask* pTask){
	LOG_DEBUG("client open success clientHandle=%d", clientHandle);
	m_pScript->callFunctionN("onOpenClientOK", clientHandle, 0);
}
void LuaHandler::onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask){
	LOG_DEBUG("client open processing callbackID=%d clientHandle=%d", callbackID, clientHandle);
	m_pScript->callFunctionNN("onOpenClient", callbackID, clientHandle, 0);
}
void LuaHandler::onOpenSocketListener(uint32 callbackID, uint32 listenerHandle, OpenSocketListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);
	m_pScript->callFunctionNN("onOpenSocketListener", callbackID, listenerHandle, 0);
}
void LuaHandler::onOpenHttpListener(uint32 callbackID, uint32 listenerHandle, OpenHttpListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);
	m_pScript->callFunctionNN("onOpenHttpListener", callbackID, listenerHandle, 0);
}
void LuaHandler::onOpenHttpsListener(uint32 callbackID, uint32 listenerHandle, OpenHttpsListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);
	m_pScript->callFunctionNN("onOpenHttpsListener", callbackID, listenerHandle, 0);
}
void LuaHandler::onCloseListener(uint32 callbackID, uint32 listenerHandle, CloseListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);
	m_pScript->callFunctionNN("onCloseListener", callbackID, listenerHandle, 0);
}
void LuaHandler::onCloseConnect(uint32 callbackID, uint32 connectHandle, CloseConnectTask* pTask){
	LOG_DEBUG("callbackID=%d connectHandle=%d", callbackID, connectHandle);
	m_pScript->callFunctionNN("onCloseConnect", callbackID, connectHandle, 0);
}
void LuaHandler::onInitialize(const std::string& param){
	LOG_DEBUG("onInitialize called handle=%d param=%s", getHandle(), param.c_str());
	initializeHandler();
	m_pScript->requireFile(param);
	m_pScript->callFunctionUR("onInitialize", this, "LuaHandler", param.c_str(), param.length(), 0);
}
void LuaHandler::onDestroy(void){
	LOG_DEBUG("onDestroy called handle=%d", getHandle());
	m_pScript->callFunction("onDestroy", 0);
	destroyHandler();
}
int64 LuaHandler::onTimerUpdate(uint32 callbackID){
//	LOG_DEBUG("callbackID=%d", callbackID);
	m_pScript->callFunctionN("onTimerUpdate", callbackID, 0);
	return -1;
}
void LuaHandler::initializeHandler(void){
	if(NULL == m_pDBRedisPool){
		m_pDBRedisPool = new DBRedisPool();
		m_pDBRedisPool->retain();
	}
	if(NULL == m_pDBMysqlPool){
		m_pDBMysqlPool = new DBMysqlPool();
		m_pDBMysqlPool->retain();
	}
	if(NULL == m_pMD5Pool){
		m_pMD5Pool = new MD5Pool();
		m_pMD5Pool->retain();
	}
	if(NULL == m_pScript){
		m_pScript = new Script();
		m_pScript->retain();
    	m_pScript->setState(NULL);
	}
}
void LuaHandler::destroyHandler(void){
	SAFE_RELEASE(m_pScript);
	SAFE_RELEASE(m_pDBRedisPool);
	SAFE_RELEASE(m_pDBMysqlPool);
	SAFE_RELEASE(m_pMD5Pool);
}

NS_HIVE_END
