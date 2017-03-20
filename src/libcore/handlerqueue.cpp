//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/30
// Time: 下午9:16
// To change this template use File | Settings | File Templates.
//

#include "handlerqueue.h"

NS_HIVE_BEGIN

static HandlerQueue* g_pHandlerQueue = NULL;
HandlerQueue::HandlerQueue() : RefObject(), Sync() {

}
HandlerQueue::~HandlerQueue(){
	releaseWorker();
	releaseHandler();
	pthread_cond_destroy(&m_cond);
}
HandlerQueue* HandlerQueue::getInstance(void){
	return g_pHandlerQueue;
}
HandlerQueue* HandlerQueue::createInstance(void){
	if(g_pHandlerQueue == NULL){
		g_pHandlerQueue = new HandlerQueue();
		SAFE_RETAIN(g_pHandlerQueue)
	}
	return g_pHandlerQueue;
}
void HandlerQueue::destroyInstance(void){
	if(NULL != g_pHandlerQueue){
		g_pHandlerQueue->releaseWorker();
    	g_pHandlerQueue->releaseHandler();
	}
    SAFE_RELEASE(g_pHandlerQueue)
}
void HandlerQueue::releaseHandler(void){
	for( auto pHandler : m_queue ){
		pHandler->release();
	}
	m_queue.clear();
}
void HandlerQueue::releaseWorker(void){
	for( auto pWorker : m_workers ){
		pWorker->cancelThread();
		pWorker->release();
	}
	m_workers.clear();
	this->broadcast();	// wake up workers
}
void HandlerQueue::createWorker(int workerNumber){
	if( m_workers.size() > 0 ){
		return;
	}
	fprintf(stderr, "--HandlerQueue::createWorker number=%d\n", workerNumber);
	pthread_cond_init(&m_cond, NULL);
	for(int i=0; i<workerNumber; ++i){
		Worker* pWorker = new Worker(this);
		m_workers.push_back(pWorker);
		if( !pWorker->startThread() ){
			fprintf(stderr, "ERROR HandlerQueue::createWorker failed index=%d\n", i);
		}else{
			fprintf(stderr, "--HandlerQueue::createWorker success index=%d\n", i);
		}
	}
}
void HandlerQueue::acceptHandler(Handler* pHandler){
	pHandler->retain();
	this->lock();
	m_queue.push_back(pHandler);
	this->signal();	// wake up workers
	this->unlock();
}
Handler* HandlerQueue::nextHandler(void){
	Handler* pHandler;
	if( m_queue.empty() ){
		return NULL;
	}else{
		pHandler = m_queue.front();
		m_queue.pop_front();
	}
	return pHandler;
}

NS_HIVE_END
