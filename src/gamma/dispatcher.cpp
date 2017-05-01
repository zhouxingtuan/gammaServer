//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/1
// Time: 下午8:39
// To change this template use File | Settings | File Templates.
//

#include "dispatcher.h"
#include "globalhandler.h"

NS_HIVE_BEGIN

Dispatcher::Dispatcher(void) : RefObject(), Sync(), m_listeners(COMMAND_NUMBER) {
	for(auto &info : m_listeners){
		info.index = 0;
	}
}
Dispatcher::~Dispatcher(void){

}

static Dispatcher* g_pDispatcher = NULL;
Dispatcher* Dispatcher::getInstance(void){
	return g_pDispatcher;
}
Dispatcher* Dispatcher::createInstance(void){
	if(g_pDispatcher == NULL){
		g_pDispatcher = new Dispatcher();
		SAFE_RETAIN(g_pDispatcher)
	}
	return g_pDispatcher;
}
void Dispatcher::destroyInstance(void){
    SAFE_RELEASE(g_pDispatcher)
}

void Dispatcher::removeCommandListener(uint32 command, uint32 handle){
	HandlerVector::iterator itCur,itEnd;
	lock();
	if(command >= m_listeners.size()){
		unlock();
		LOG_ERROR("command is larger then COMMAND_NUMBER=%d command=%d", COMMAND_NUMBER, command);
		return;
	}
	HandlerVector& vec = m_listeners[command].handles;
	itCur = vec.begin();
	itEnd = vec.end();
	for(; itCur != itEnd; ++itCur){
		if(*itCur == handle){
			vec.erase(itCur);
			unlock();
			return;
		}
	}
	unlock();
}
bool Dispatcher::appendCommandListener(uint32 command, uint32 handle){
	lock();
	if(command >= m_listeners.size()){
		unlock();
		LOG_ERROR("command is larger then COMMAND_NUMBER=%d command=%d", COMMAND_NUMBER, command);
		return false;
	}
	HandlerVector& vec = m_listeners[command].handles;
	for(auto h : vec){
		if(h == handle){
			unlock();
			return false;
		}
	}
	vec.push_back(handle);
	unlock();
	return true;
}
bool Dispatcher::dispatchCommand(Packet* pPacket, uint32 command){
	uint32 handle = getCommandHandle(command);
	if(handle == 0){
		LOG_ERROR("dispatchCommand can not find available Handler command=%d", command);
	}else{
		return GlobalHandler::getInstance()->dispatchToHandler(pPacket, handle);
	}
	return false;
}
uint32 Dispatcher::getCommandHandle(uint32 command){
	uint32 handle = 0;
	lock();
	if(command < (uint32)m_listeners.size()){
		CommandHandlerInfo& info = m_listeners[command];
		HandlerVector& vec = info.handles;
		if(vec.size() == 1){
			handle = vec[0];
		}else if(vec.size() > 1){
			uint32 index = info.index + 1;
			if(index >= (uint32)vec.size()){
				info.index = 0;
				handle = vec[0];
			}else{
				info.index = index;
				handle = vec[index];
			}
		}
	}
	unlock();
	return handle;
}

NS_HIVE_END
