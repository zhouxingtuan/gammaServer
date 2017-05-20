//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/13
// Time: 下午9:14
// To change this template use File | Settings | File Templates.
//

#include "luahandler.h"

NS_HIVE_BEGIN

LuaHandler::LuaHandler(void) : Handler(), m_pScript(NULL) {

}
LuaHandler::~LuaHandler(void){
	SAFE_RELEASE(m_pScript);
}

void LuaHandler::onReceivePacket(Packet* pPacket, Task* pTask){
	LOG_DEBUG("packet length=%d", pPacket->getLength());

}
void LuaHandler::onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK){
	LOG_DEBUG("buffer length=%d callbackID=%d isRequestOK=%d", pBuffer->size(), callbackID, isRequestOK);

}
void LuaHandler::onOpenClientOK(uint32 clientHandle, OpenClientOKTask* pTask){
	LOG_DEBUG("client open success clientHandle=%d", clientHandle);

}
void LuaHandler::onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask){
	LOG_DEBUG("client open processing callbackID=%d clientHandle=%d", callbackID, clientHandle);

}
void LuaHandler::onOpenSocketListener(uint32 callbackID, uint32 listenerHandle, OpenSocketListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);

}
void LuaHandler::onOpenHttpListener(uint32 callbackID, uint32 listenerHandle, OpenHttpListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);

}
void LuaHandler::onOpenHttpsListener(uint32 callbackID, uint32 listenerHandle, OpenHttpsListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);

}
void LuaHandler::onCloseListener(uint32 callbackID, uint32 listenerHandle, CloseListenerTask* pTask){
	LOG_DEBUG("callbackID=%d listenerHandle=%d", callbackID, listenerHandle);

}
void LuaHandler::onCloseConnect(uint32 callbackID, uint32 connectHandle, CloseConnectTask* pTask){
	LOG_DEBUG("callbackID=%d connectHandle=%d", callbackID, connectHandle);

}
void LuaHandler::onInitialize(const std::string& param){
	LOG_DEBUG("onInitialize called handle=%d param=%s", getHandle(), param.c_str());
	if(NULL == m_pScript){
		m_pScript = new Script();
		m_pScript->retain();
    	m_pScript->setState(NULL);
    	m_pScript->requireFile(param);
	}
}
void LuaHandler::onDestroy(void){
	LOG_DEBUG("onDestroy called handle=%d", getHandle());
}
int64 LuaHandler::onTimerUpdate(uint32 callbackID){
	LOG_DEBUG("callbackID=%d", callbackID);

	return -1;
}

NS_HIVE_END
