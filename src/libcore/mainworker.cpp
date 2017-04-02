//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/2/26
// Time: 下午8:54
// To change this template use File | Settings | File Templates.
//

#include "mainworker.h"

NS_HIVE_BEGIN

MainWorker::MainWorker(void) : ActiveWorker(0), m_pListenerPool(NULL) {

}
MainWorker::~MainWorker(void){
	MainWorker::destroy();
}

static MainWorker* g_pMainWorker = NULL;
MainWorker* MainWorker::getInstance(void){
	return g_pMainWorker;
}
MainWorker* MainWorker::createInstance(void){
	if(g_pMainWorker == NULL){
		g_pMainWorker = new MainWorker();
		SAFE_RETAIN(g_pMainWorker)
	}
	return g_pMainWorker;
}
void MainWorker::destroyInstance(void){
    SAFE_RELEASE(g_pMainWorker)
}

bool MainWorker::closeListener(uint32 handle){
	LOG_DEBUG("handle=%d", handle);
	Listener* pListener = getListener(handle);
	if(NULL == pListener){
		return false;
	}
	m_pEpoll->objectRemove(pListener);
	pListener->resetData();
	m_pListenerPool->idle(handle);
	LOG_DEBUG("OK handle=%d", handle);
	return true;
}
uint32 MainWorker::openListener(const char* ip, uint16 port, AcceptSocketFunction pFunc, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex){
	LOG_DEBUG("try to open Listener ip=%s port=%d", ip, port);
	Listener* pListener = (Listener*)m_pListenerPool->create();
	if(NULL == pListener){
		LOG_ERROR("NULL == pListener");
		return 0;
	}
	uint32 handle = pListener->getHandle();
	pListener->setEpoll(m_pEpoll);
	pListener->setSocket(ip, port);
	pListener->setAcceptSocketFunction(pFunc);
	pListener->setAcceptIndex(acceptIndex);
	if( !pListener->openSocket() ){
		closeListener(handle);
		LOG_ERROR("Listener openSocket failed");
		return 0;
	}
	if( !m_pEpoll->objectAdd(pListener, EPOLLIN) ){
		pListener->closeSocket();
		closeListener(handle);
		LOG_ERROR("Listener objectAdd to epoll failed");
		return 0;
	}
	pListener->setIsNeedEncrypt(isNeedEncrypt);
	pListener->setIsNeedDecrypt(isNeedDecrypt);
	LOG_INFO("handle=%d fd=%d ip=%s port=%d", handle, pListener->getSocketFD(), ip, port);
	return handle;
}
uint32 MainWorker::openHttpListener(const char* ip, uint16 port){
	return this->openListener(ip, port, MainWorker::onAcceptHttp, false, false, 0);
}
uint32 MainWorker::openHttpsListener(const char* ip, uint16 port){
	return this->openListener(ip, port, MainWorker::onAcceptHttps, false, false, 0);
}
uint32 MainWorker::openSocketListener(const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex){
	return this->openListener(ip, port, MainWorker::onAcceptSocket, isNeedEncrypt, isNeedDecrypt, acceptIndex);
}

void MainWorker::onAcceptSocket(int fd, const char* ip, uint16 port, Listener* pListener){
	OpenAcceptTask* pTask = new OpenAcceptTask();
	pTask->retain();
	pTask->m_fd = fd;
	pTask->setSocket(ip, port);
	pTask->m_isNeedEncrypt = pListener->isNeedEncrypt();
	pTask->m_isNeedDecrypt = pListener->isNeedDecrypt();
	pTask->m_acceptIndex = pListener->getAcceptIndex();
	GlobalService::getInstance()->dispatchTaskEqualToEpollWorker(pTask);
	pTask->release();
}
void MainWorker::onAcceptHttp(int fd, const char* ip, uint16 port, Listener* pListener){
	OpenHttpTask* pTask = new OpenHttpTask();
	pTask->retain();
	pTask->m_fd = fd;
	pTask->setSocket(ip, port);
	GlobalService::getInstance()->dispatchTaskEqualToEpollWorker(pTask);
	pTask->release();
}
void MainWorker::onAcceptHttps(int fd, const char* ip, uint16 port, Listener* pListener){
	OpenHttpsTask* pTask = new OpenHttpsTask();
	pTask->retain();
	pTask->m_fd = fd;
	pTask->setSocket(ip, port);
	GlobalService::getInstance()->dispatchTaskEqualToEpollWorker(pTask);
	pTask->release();
}

void MainWorker::update(void){
	LOG_INFO("start main serviceID=%d", getServiceID());
	int64 timeout;
	while(1){
		timeout = m_pTimer->getWaitTimeout();
		m_pEpoll->update(timeout);
		m_pTimer->update();
	}
	LOG_INFO("exit main serviceID=%d", getServiceID());
}

void MainWorker::initialize(uint32 nodeID, uint32 epollWorkerNumber, uint32 workerNumber){
	// 初始化基类数据
	ActiveWorker::initialize();

	GlobalSetting::createInstance()->initialize(nodeID);
	if(NULL == m_pListenerPool){
		m_pListenerPool = new DestinationPool();
		m_pListenerPool->retain();
		m_pListenerPool->registerFunction(nodeID, getServiceID(), 0, Listener::createObject, Listener::releaseObject);
	}
	TimerManager::createInstance()->setTimer(m_pTimer);
	GlobalHandler::createInstance()->initialize();
	GlobalService::createInstance()->initialize(epollWorkerNumber);
	HandlerQueue::createInstance()->createWorker(workerNumber);
}
void MainWorker::destroy(void){
	SAFE_RELEASE(m_pListenerPool)

	GlobalService::destroyInstance();
	HandlerQueue::destroyInstance();
}

NS_HIVE_END
