//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/2
// Time: 下午11:01
// To change this template use File | Settings | File Templates.
//

#include "activeworker.h"

NS_HIVE_BEGIN

ActiveWorker::ActiveWorker(uint32 serviceID) : Active(), RefObject(), Sync(),
	m_pEpoll(NULL), m_pTimer(NULL), m_serviceID(serviceID) {

}
ActiveWorker::~ActiveWorker(void){
	ActiveWorker::destroy();
}

void ActiveWorker::onActive(uint64 value){
	this->lock();
	if(m_taskQueue.size() > 0){
		m_tempQueue.swap(m_taskQueue);
	}
	this->unlock();
	if(m_tempQueue.size() > 0){
		for(auto pTask : m_tempQueue){
			pTask->doActiveTask(this); 	// 执行任务
			pTask->release();
		}
		m_tempQueue.clear();
	}
}
void ActiveWorker::acceptTask(Task* pTask){
//	LOG_DEBUG("ActiveWorker accept Task handle=%d", getHandle());
	pTask->retain();
	this->lock();
	m_taskQueue.push_back(pTask);
	this->unlock();
	this->active();
}
void ActiveWorker::initialize(void){
	if(NULL == m_pEpoll){
		m_pEpoll = new Epoll();
		m_pEpoll->retain();
		m_pEpoll->setWorker(this);	// 设置Worker指针
		if( !m_pEpoll->createEpoll() ){
			LOG_ERROR("createEpoll failed");
		}
	}
	if(NULL == m_pTimer){
		m_pTimer = new Timer();
		m_pTimer->retain();
		m_pTimer->start(MAX_0824_HASH_SIZE);
	}
	// 把自己加入到Epoll监听中
	if( !createEvent(m_pEpoll) ){
		LOG_ERROR("createEvent failed");
	}
}
void ActiveWorker::destroy(void){
	for(auto pTask : m_taskQueue){
		pTask->release();
	}
	m_taskQueue.clear();
	for(auto pTask : m_tempQueue){
		pTask->release();
	}
	m_tempQueue.clear();
	SAFE_RELEASE(m_pEpoll)
	SAFE_RELEASE(m_pTimer)
}

NS_HIVE_END
