//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/2
// Time: 下午10:57
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__activeworker__
#define __hive__activeworker__

#include "worker.h"
#include "accept.h"
#include "client.h"
#include "http.h"
#include "https.h"
#include "active.h"
#include "listener.h"


NS_HIVE_BEGIN

class ActiveWorker : public Active, public RefObject, public Sync
{
public:
	typedef std::deque<Task*> TaskQueue;
public:
	TaskQueue m_taskQueue;
	TaskQueue m_tempQueue;
	Epoll* m_pEpoll;
	Timer* m_pTimer;
	uint16 m_serviceID;
public:
	ActiveWorker(uint32 serviceID);
	virtual ~ActiveWorker(void);

	// from Active
	virtual void onActive(uint64 value);

	virtual void acceptTask(Task* pTask);

	virtual void initialize(void);
	virtual void destroy(void);

	inline Epoll* getEpoll(void){ return m_pEpoll; }
	inline Timer* getTimer(void){ return m_pTimer; }
	inline uint16 getServiceID(void){ return m_serviceID; }
	inline void setServiceID(uint32 serviceID){ m_serviceID = serviceID; }
};

NS_HIVE_END

#endif
