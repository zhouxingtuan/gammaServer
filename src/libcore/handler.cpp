//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/7
// Time: 下午9:31
// To change this template use File | Settings | File Templates.
//

#include "handler.h"
#include "handlerqueue.h"
#include "epollworker.h"
#include "mainworker.h"
#include "timerhandle.h"
#include "globalservice.h"

NS_HIVE_BEGIN

Handler::Handler(void) : Destination(), RefObject(), Sync(), m_isInHandlerQueue(false) {

}
Handler::~Handler(void){
	releaseTask();
}

void MainHandler::identifyHive(uint32 connectHandle){
	char temp[256] = {0};
	uint32 nodeID = GlobalSetting::getInstance()->getNodeID();
	uint32 t = time(NULL);
	const std::string& password = GlobalSetting::getInstance()->getPassword();
	sprintf(temp, "%04d-%d-%s", nodeID, t, password.c_str());
	uint64 magic = binary_hash64(temp, strlen(temp));
	LOG_DEBUG("identifyHive to connectHandle=%d nodeID=%d str=%s magic=%llu", connectHandle, nodeID, temp, magic);
	Packet* pPacket = new Packet(PACKET_HEAD_LENGTH + 16);
	pPacket->retain();
	pPacket->writeBegin(COMMAND_REGISTER, connectHandle);
	pPacket->write(&nodeID, sizeof(uint32));
	pPacket->write(&t, sizeof(uint32));
	pPacket->write(&magic, sizeof(uint64));
	pPacket->writeEnd();
	bool result = GlobalService::getInstance()->sendToService(connectHandle, pPacket);
	LOG_DEBUG("identifyHive to connectHandle=%d command=%d result=%d", connectHandle, pPacket->getCommand(), result);
	pPacket->release();
}
void Handler::openClient(uint32 callbackID, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex){
	OpenClientTask* pTask = new OpenClientTask();
	pTask->retain();
	pTask->m_callbackID = callbackID;
	pTask->m_bindHandle = this->getHandle();
	pTask->setSocket(ip, port);
	pTask->m_isNeedEncrypt = isNeedEncrypt;
	pTask->m_isNeedDecrypt = isNeedDecrypt;
	pTask->m_acceptIndex = acceptIndex;
	GlobalService::getInstance()->dispatchTaskEqualToEpollWorker(pTask);
	pTask->release();
}
void Handler::closeConnect(uint32 callbackID, uint32 connectHandle){
	CloseConnectTask* pTask = new CloseConnectTask();
	pTask->retain();
	pTask->m_callbackID = callbackID;
	pTask->m_bindHandle = this->getHandle();
	pTask->m_connectHandle = connectHandle;
	GlobalService::getInstance()->dispatchTaskToEpollWorkerHandle(connectHandle, pTask);
	pTask->release();
}
void Handler::bindAccept(uint32 acceptHandle, uint32 bindHandle){
	BindAcceptHandleTask* pTask = new BindAcceptHandleTask();
	pTask->retain();
	pTask->m_acceptHandle = acceptHandle;
	pTask->m_bindHandle = bindHandle;
	GlobalService::getInstance()->dispatchTaskToEpollWorkerHandle(acceptHandle, pTask);
	pTask->release();
}
void Handler::openHttpListener(uint32 callbackID, const char* ip, uint16 port){
	OpenHttpListenerTask* pTask = new OpenHttpListenerTask();
	pTask->retain();
	pTask->m_callbackID = callbackID;
	pTask->m_bindHandle = this->getHandle();
	pTask->setSocket(ip, port);
	MainWorker::getInstance()->acceptTask(pTask);
	pTask->release();
}
void Handler::openHttpsListener(uint32 callbackID, const char* ip, uint16 port){
	OpenHttpsListenerTask* pTask = new OpenHttpsListenerTask();
	pTask->retain();
	pTask->m_callbackID = callbackID;
	pTask->m_bindHandle = this->getHandle();
	pTask->setSocket(ip, port);
	MainWorker::getInstance()->acceptTask(pTask);
	pTask->release();
}
void Handler::openSocketListener(uint32 callbackID, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex){
	OpenSocketListenerTask* pTask = new OpenSocketListenerTask();
	pTask->retain();
	pTask->m_callbackID = callbackID;
	pTask->m_bindHandle = this->getHandle();
	pTask->setSocket(ip, port);
	pTask->m_isNeedEncrypt = isNeedEncrypt;
	pTask->m_isNeedDecrypt = isNeedDecrypt;
	pTask->m_acceptIndex = acceptIndex;
	MainWorker::getInstance()->acceptTask(pTask);
	pTask->release();
}
void Handler::closeListener(uint32 callbackID, uint32 listenerHandle){
	CloseListenerTask* pTask = new CloseListenerTask();
	pTask->retain();
	pTask->m_callbackID = callbackID;
	pTask->m_bindHandle = this->getHandle();
	pTask->m_listenerHandle = listenerHandle;
	MainWorker::getInstance()->acceptTask(pTask);
	pTask->release();
}
void Handler::sendCurlRequest(RequestData* pRequest){
	GlobalService::getInstance()->sendCurlRequest(pRequest);
}

int64 Handler::timerActiveCallback(uint32 callbackID){
	TimerUpdateTask* pTask = new TimerUpdateTask();
	pTask->retain();
	pTask->setCallbackID(callbackID);
	this->acceptTask(pTask);
	pTask->release();
	return -1;
}

// 开始一个计时器
uint32 Handler::startTimer(uint32 callbackID, int64 timeCount){
	LOG_DEBUG("Handler startTimer handle=%d callbackID=%d timeCount=%lld", getHandle(), callbackID, timeCount);
	return TimerManager::getInstance()->startTimerSync(callbackID, timeCount, this);
}
// 移除计时器
void Handler::removeTimer(uint32 handle){
	TimerManager::getInstance()->removeTimerSync(handle);
}
// 更改计时器的时间
void Handler::changeTimer(uint32 handle, int64 timeCount){
	TimerManager::getInstance()->changeTimerSync(handle, timeCount);
}
// 获取计时器剩余时间
int64 Handler::leftTimer(uint32 handle){
	return TimerManager::getInstance()->getTimerLeftSync(handle);
}
bool Handler::receivePacket(Packet* pPacket){
	ReceivePacketTask* pTask = new ReceivePacketTask();
	pTask->retain();
	pTask->setPacket(pPacket);
	this->acceptTask(pTask);
	pTask->release();
	return true;
}

void Handler::doHandler(HandlerQueue* pQueue){
	// 执行任务，因为已经保证handler被worker独占，所以可以不在临界区内
	this->doTask();
	if(NULL != pQueue){
		this->lock();
		if( isFinished() ){
			m_isInHandlerQueue = false;
			this->unlock();
		}else{
			m_isInHandlerQueue = true;
			this->unlock();
			pQueue->acceptHandler(this);	// 将handler提交给队列
		}
	}

}
void Handler::notify(void){
	this->lock();
	if( m_isInHandlerQueue ){	// 已经在队列里面，入队后不做任何处理
		this->unlock();
	}else{						// 原先不在队列中则重新入队
		if( isFinished() ){
			m_isInHandlerQueue = false;
			this->unlock();
		}else{
			m_isInHandlerQueue = true;
			this->unlock();
			HandlerQueue::getInstance()->acceptHandler(this);	// 将handler提交给队列
		}
	}
}
void Handler::releaseTask(void){
	LOG_DEBUG("Handler release Task handle=%d", getHandle());
	this->lock();
	for( auto pTask : m_taskQueue ){
		pTask->release();
	}
	m_taskQueue.clear();
	m_isInHandlerQueue = false;
	this->unlock();
}
void Handler::doTask(void){
	Task* pTask = NULL;
	this->lock();
	if( !m_taskQueue.empty() ){
		pTask = m_taskQueue.front();		// 这里获取第一个任务来执行
		m_taskQueue.pop_front();
	}
	this->unlock();
	if( NULL != pTask ){
		pTask->doHandlerTask(this); 	// 执行任务，因为已经保证handler被worker独占，所以可以不在临界区内
		pTask->release();
	}
}
void Handler::acceptTask(Task* pTask){
	pTask->retain();
	LOG_DEBUG("Handler accept Task handle=%d", getHandle());
	this->lock();
	m_taskQueue.push_back(pTask);
	if( m_isInHandlerQueue ){	// 已经在队列里面，入队后不做任何处理
		this->unlock();
		LOG_DEBUG("Handler accept Task when is in HandlerQueue handle=%d", getHandle());
	}else{						// 原先不在队列中则重新入队
		m_isInHandlerQueue = true;
		this->unlock();
		LOG_DEBUG("Handler accept Task when is not in HandlerQueue, and push handle=%d", getHandle());
		HandlerQueue::getInstance()->acceptHandler(this);	// 将handler提交给队
	}
}
void Handler::acceptTaskFront(Task* pTask){
	pTask->retain();
	LOG_DEBUG("Handler accept Task Front handle=%d", getHandle());
	this->lock();
	m_taskQueue.push_front(pTask);
	if( m_isInHandlerQueue ){	// 已经在队列里面，入队后不做任何处理
		this->unlock();
		LOG_DEBUG("Handler accept Task Front when is in HandlerQueue handle=%d", getHandle());
	}else{						// 原先不在队列中则重新入队
		m_isInHandlerQueue = true;
		this->unlock();
		LOG_DEBUG("Handler accept Task Front when is not in HandlerQueue, and push handle=%d", getHandle());
		HandlerQueue::getInstance()->acceptHandler(this);	// 将handler提交给队
	}
}

NS_HIVE_END
