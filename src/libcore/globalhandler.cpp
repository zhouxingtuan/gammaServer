//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/2
// Time: 上午7:16
// To change this template use File | Settings | File Templates.
//

#include "globalhandler.h"
#include "mainworker.h"

NS_HIVE_BEGIN

GlobalHandler::GlobalHandler(void) : RefObject(), Sync() ,m_pGroup(NULL) {

}
GlobalHandler::~GlobalHandler(void){

}

static GlobalHandler* g_pGlobalHandler = NULL;
GlobalHandler* GlobalHandler::getInstance(void){
	return g_pGlobalHandler;
}
GlobalHandler* GlobalHandler::createInstance(void){
	if(g_pGlobalHandler == NULL){
		g_pGlobalHandler = new GlobalHandler();
		SAFE_RETAIN(g_pGlobalHandler)
	}
	return g_pGlobalHandler;
}
void GlobalHandler::destroyInstance(void){
    SAFE_RELEASE(g_pGlobalHandler)
}

bool GlobalHandler::dispatchTask(uint32 handle, Task* pTask){
	Handler* pHandler;
	lock();
	pHandler = (Handler*)m_pGroup->getDestination(handle);
	if(NULL != pHandler){
		pHandler->retain();
	}
	unlock();
	if(NULL == pHandler){
		fprintf(stderr, "ERROR GlobalHandler dispatchTask Handler not found=%d\n", handle);
		return false;
	}
	// 生成Task并且派发消息
	pHandler->acceptTask(pTask);
	pHandler->release();
	return true;
}
bool GlobalHandler::dispatchToHandler(Packet* pPacket){
	// 查找Handler对象，将这个消息生成一个Task，派发给Handler去执行
	Handler* pHandler;
	PacketHead* pHead = pPacket->getHead();
	uint32 handle = pHead->destination.handle;
	lock();
	pHandler = (Handler*)m_pGroup->getDestination(handle);
	if(NULL != pHandler){
		pHandler->retain();
	}
	unlock();
	if(NULL == pHandler){
		fprintf(stderr, "ERROR GlobalHandler dispatchToHandler Handler not found=%d\n", handle);
		return false;
	}
	// 生成Task并且派发消息
	bool result = pHandler->receivePacket(pPacket);
	pHandler->release();
	return result;
}
int64 GlobalHandler::activeTimer(uint32 handle, uint32 callbackID){
	Handler* pHandler;
	lock();
	pHandler = (Handler*)m_pGroup->getDestination(handle);
	if(NULL != pHandler){
		pHandler->retain();
	}
	unlock();
	if(NULL == pHandler){
		fprintf(stderr, "ERROR GlobalHandler activeTimer Handler not found=%d\n", handle);
		return false;
	}
	int64 timeCount = pHandler->timerActiveCallback(callbackID);
	pHandler->release();
	return timeCount;
}
void GlobalHandler::initialize(void){
	if(NULL == m_pGroup){
		uint32 nodeID = MainWorker::getInstance()->getNodeID();
		m_pGroup = new DestinationGroup(nodeID, 0);
		m_pGroup->retain();
	}
}
void GlobalHandler::destroy(void){
	SAFE_RELEASE(m_pGroup)
}

NS_HIVE_END
