//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/2
// Time: 上午7:13
// To change this template use File | Settings | File Templates.
//

#include "globalservice.h"
#include "globalsetting.h"

NS_HIVE_BEGIN

GlobalService::GlobalService(void) : RefObject(), Sync(), m_epollMagic(0), m_epollWorkerNumber(0) {

}
GlobalService::~GlobalService(void){
	GlobalService::destroy();
}

static GlobalService* g_pGlobalService = NULL;
GlobalService* GlobalService::getInstance(void){
	return g_pGlobalService;
}
GlobalService* GlobalService::createInstance(void){
	if(g_pGlobalService == NULL){
		g_pGlobalService = new GlobalService();
		SAFE_RETAIN(g_pGlobalService)
	}
	return g_pGlobalService;
}
void GlobalService::destroyInstance(void){
    SAFE_RELEASE(g_pGlobalService)
}

void GlobalService::sendCurlRequest(RequestData* pRequest){
	uint32 service = this->nextEpollMagic() % m_epollWorkerNumber + 1;	// 下标从1开始
	EpollWorker* pWorker;
	lock();
	pWorker = getWorker(service);
	if(NULL != pWorker){
		pWorker->retain();
	}
	unlock();
	if(NULL == pWorker){
		LOG_ERROR("EpollWorker not found=%d", service);
		return;
	}
	pWorker->sendCurlRequest(pRequest);
	pWorker->release();
}
bool GlobalService::dispatchTaskEqualToEpollWorker(Task* pTask){
	uint32 service = this->nextEpollMagic() % m_epollWorkerNumber + 1;	// 下标从1开始
	return dispatchTaskToEpollWorker(service, pTask);
}
bool GlobalService::dispatchTaskToEpollWorkerHandle(uint32 handle, Task* pTask){
	DestinationHandle h(handle);
	uint32 service = h.service;
	if(0 == service){
		return false;
	}
	return dispatchTaskToEpollWorker(service, pTask);
}
bool GlobalService::dispatchTaskToEpollWorker(uint32 service, Task* pTask){
	// 分发消息给EpollWorker
	EpollWorker* pWorker;
	lock();
	pWorker = getWorker(service);
	if(NULL != pWorker){
		pWorker->retain();
	}
	unlock();
	if(NULL == pWorker){
		LOG_ERROR("EpollWorker not found=%d", service);
		return false;
	}
	pWorker->acceptTask(pTask);
	pWorker->release();
	return true;
}
bool GlobalService::dispatchTask(uint32 handle, Task* pTask){
	DestinationHandle h(handle);
	uint32 service = h.service;
	if(0 == service){
		return GlobalHandler::getInstance()->dispatchTask(handle, pTask);
	}else{
		return dispatchTaskToEpollWorker(service, pTask);
	}
}
bool GlobalService::sendToNode(uint32 nodeID, Packet* pPacket){
	if(0 == nodeID || GlobalSetting::getInstance()->getNodeID() == nodeID){
		return sendToService(pPacket->getHead()->destination.handle, pPacket);
	}
	// 发送消息给其它网络节点
	uint32 handle = getNodeConnect(nodeID);
	if(0 == handle){
		LOG_ERROR("node connect not found=%d", nodeID);
		return false;
	}
	return dispatchToEpollWorker(handle, pPacket);
}
bool GlobalService::sendToService(uint32 handle, Packet* pPacket){
	DestinationHandle h(handle);
	if(0 == h.service){
		// 分发消息给GlobalHandler
		return GlobalHandler::getInstance()->dispatchToHandler(pPacket, handle);
	}else{
		return dispatchToEpollWorker(handle, pPacket);
	}
}
bool GlobalService::dispatchToService(Packet* pPacket){
	return sendToNode(pPacket->getHead()->destination.node, pPacket);
}
bool GlobalService::dispatchToEpollWorker(uint32 handle, Packet* pPacket){
	DestinationHandle h(handle);
	uint32 service = h.service;
	// 分发消息给EpollWorker
	EpollWorker* pWorker;
	lock();
	pWorker = getWorker(service);
	if(NULL != pWorker){
		pWorker->retain();
	}
	unlock();
	if(NULL == pWorker){
		LOG_ERROR("EpollWorker not found=%d", service);
		return false;
	}
	bool result = pWorker->dispatchToConnect(handle, pPacket);
	pWorker->release();
	return result;
}
void GlobalService::initialize(uint16 epollWorkerNumber){
	if(epollWorkerNumber > MAX_EPOLL_WORKER_NUMBER){
		LOG_ERROR("epoll worker only support=%d reset from=%d", MAX_EPOLL_WORKER_NUMBER, epollWorkerNumber);
		epollWorkerNumber = MAX_EPOLL_WORKER_NUMBER;
	}
	m_epollWorkerNumber = epollWorkerNumber;
	// 不使用下标为0的数据，0代表主线程
	m_epollWorkers.resize(epollWorkerNumber + 1, NULL);
	for(uint16 i = 1; i <= epollWorkerNumber; ++i){
		EpollWorker* pWorker = new EpollWorker(i);
		pWorker->retain();
		pWorker->initialize();  // init right after create
		if(!pWorker->startThread()){
			LOG_ERROR("EpollWorker startThread failed serviceID=%d", i);
		}
		m_epollWorkers[i] = pWorker;
	}
	m_nodeConnects.resize(MAX_NODE_NUMBER + 1, 0);
}
void GlobalService::destroy(void){
	for(auto pWorker : m_epollWorkers){
		if(NULL != pWorker){
			pWorker->release();
		}
	}
	m_epollWorkers.clear();
	m_nodeConnects.clear();
}

NS_HIVE_END
