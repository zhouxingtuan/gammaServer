//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/24
// Time: 上午1:21
// To change this template use File | Settings | File Templates.
//

#include "timerhandle.h"
#include "mainworker.h"

NS_HIVE_BEGIN

TimerHandle::~TimerHandle(void){

}
int64 TimerHandle::timerCallback(void){
	int64 timeCount = GlobalHandler::getInstance()->activeTimer(m_handlerID, m_callbackID);
	if( -1 == timeCount ){
		TimerManager::getInstance()->removeTimerHandle(this);
	}
	return timeCount;
}

TimerManager::TimerManager(void) : RefObject(), Sync(), m_pTimer(NULL), m_pTimerHandlePool(NULL) {

}
TimerManager::~TimerManager(void){
	TimerManager::destroy();
}
static TimerManager* g_pTimerManager = NULL;
TimerManager* TimerManager::getInstance(void){
	return g_pTimerManager;
}
TimerManager* TimerManager::createInstance(void){
	if(g_pTimerManager == NULL){
		g_pTimerManager = new TimerManager();
		g_pTimerManager->initialize();
		SAFE_RETAIN(g_pTimerManager)
	}
	return g_pTimerManager;
}
void TimerManager::destroyInstance(void){
    SAFE_RELEASE(g_pTimerManager)
}

bool TimerManager::initialize(void){
	m_pTimerHandlePool = new TimerHandlePool();
	SAFE_RETAIN(m_pTimerHandlePool)
	return true;
}
void TimerManager::destroy(void){
	SAFE_RELEASE(m_pTimerHandlePool)
}

// 异步 开始一个计时器
uint32 TimerManager::startTimerSync(uint32 callbackID, int64 timeCount, Handler* pHandler){
	LOG_DEBUG("handle=%d callbackID=%d timeCount=%lld", pHandler->getHandle(), callbackID, timeCount);
	TimerHandle* pHandle = this->createTimerHandle();
	if(NULL == pHandle){
		LOG_ERROR("not enough time handle handle=%d callbackID=%d timeCount=%lld", pHandler->getHandle(), callbackID, timeCount);
		return 0;
	}
	uint32 handle = pHandle->getHandle();
	StartTimerTask* pTask = new StartTimerTask();
	pTask->retain();
	pTask->setTimerHandle(handle);
	pTask->setCallbackID(callbackID);
	pTask->setTimeCount(timeCount);
	pTask->setActiveObject(pHandler->getHandle());
	LOG_DEBUG("send task to main handle=%d callbackID=%d timeCount=%lld", pHandler->getHandle(), callbackID, timeCount);
	MainWorker::getInstance()->acceptTask(pTask);
	pTask->release();
	return handle;
}
// 异步 移除计时器
void TimerManager::removeTimerSync(uint32 handle){
	RemoveTimerTask* pTask = new RemoveTimerTask();
	pTask->retain();
	pTask->setTimerHandle(handle);
	MainWorker::getInstance()->acceptTask(pTask);
	pTask->release();
}
// 异步 更改计时器的时间
void TimerManager::changeTimerSync(uint32 handle, int64 timeCount){
	ChangeTimerTask* pTask = new ChangeTimerTask();
	pTask->retain();
	pTask->setTimerHandle(handle);
	pTask->setTimeCount(timeCount);
	MainWorker::getInstance()->acceptTask(pTask);
	pTask->release();
}
// 异步 获取计时器剩余时间
int64 TimerManager::getTimerLeftSync(uint32 handle){
	int64 time_left;
	lock();
	time_left = this->getTimerLeft(handle);
	unlock();
	return time_left;
}

// 开始一个计时器
uint32 TimerManager::startTimer(uint32 callbackID, int64 timeCount, Handler* pHandler){
	TimerHandle* pHandle =  this->createTimerHandle();
	if(NULL == pHandle){
		return 0;
	}
	pHandle->setCallbackID(callbackID);
	pHandle->setActiveObject(pHandler->getHandle());
	pHandle->setTimer(timeCount, m_pTimer);
	return pHandle->getHandle();
}
// 移除计时器
void TimerManager::removeTimer(uint32 handle){
	TimerHandle* pHandle = getTimerHandle(handle);
	if(NULL != pHandle){
		this->removeTimerHandle(pHandle);
	}
}
// 更改计时器的时间
void TimerManager::changeTimer(uint32 handle, int64 timeCount){
	TimerHandle* pHandle = getTimerHandle(handle);
	if(NULL != pHandle){
		pHandle->setTimer(timeCount, m_pTimer);
	}
}
// 获取计时器剩余时间
int64 TimerManager::getTimerLeft(uint32 handle){
	TimerHandle* pHandle = getTimerHandle(handle);
	if(NULL != pHandle){
		return pHandle->getTimerLeft();
	}
	return 0;
}


NS_HIVE_END
