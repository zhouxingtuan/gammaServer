//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/30
// Time: 下午9:16
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__handlerqueue__
#define __hive__handlerqueue__

#include "handler.h"
#include "worker.h"

NS_HIVE_BEGIN

class HandlerQueue : public RefObject, public Sync
{
public:
	friend class Worker;
	friend class Handler;
public:
	typedef std::deque<Handler*> HandlerInterfaceQueue;
	typedef std::vector<Worker*> WorkerVector;
	HandlerQueue(void);
	virtual ~HandlerQueue(void);

	static HandlerQueue* getInstance(void);
	static HandlerQueue* createInstance(void);
	static void destroyInstance(void);

	void createWorker(int workerNumber);
	void releaseWorker(void);
	void releaseHandler(void);
	inline void signal(void){
		pthread_cond_signal(&m_cond);
	}
	inline void wait(void){
		pthread_cond_wait(&m_cond, &m_mutex);
	}
	inline void broadcast(void){
		pthread_cond_broadcast(&m_cond);
	}
protected:
	void acceptHandler(Handler* pHandler);		// Handler/HandlerInterface 调用
	Handler* nextHandler(void);					// Worker 调用
protected:
	pthread_cond_t m_cond;
	HandlerInterfaceQueue m_queue;
	WorkerVector m_workers;
};

NS_HIVE_END

#endif
