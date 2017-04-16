//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/24
// Time: 上午1:21
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__timerhandle__
#define __hive__timerhandle__

#include "timer.h"
#include "object.h"
#include "objectpool.h"

NS_HIVE_BEGIN

class Handler;

class TimerHandle : public TimerObject, public Object0824
{
public:
	TimerHandle(void) : TimerObject(), Object0824(), m_handlerID(0), m_callbackID(0){}
	virtual ~TimerHandle(void);

	static TimerHandle* createObject(void){
		TimerHandle* pHandle = new TimerHandle();
		return pHandle;
	}
	static void releaseObject(TimerHandle* pHandle){
		if(NULL != pHandle){
			delete pHandle;
		}
	}

	// from TimerObject
	virtual int64 timerCallback(void);

	inline uint32 getCallbackID(void) const { return m_callbackID; }
	inline void setCallbackID(uint32 id){ m_callbackID = id; }
	void setActiveObject(uint32 handlerID){ m_handlerID = handlerID; }
public:
	uint32 m_handlerID;
	uint32 m_callbackID;				// 返回的callback ID
};

typedef ObjectPool<TimerHandle> TimerHandlePool;

class TimerManager : public RefObject, public Sync
{
protected:
	Timer* 							m_pTimer;					// 计时器管理指针
	TimerHandlePool*				m_pTimerHandlePool;			// 计时器对象池
public:
	TimerManager(void);
	virtual ~TimerManager(void);

	static TimerManager* getInstance(void);
	static TimerManager* createInstance(void);
	static void destroyInstance(void);

	void setTimer(Timer* pTimer){ m_pTimer = pTimer; }
	Timer* getTimer(void){ return m_pTimer; }

	uint32 startTimerSync(uint32 callbackID, int64 timeCount, Handler* pHandler);		// 异步 开始一个计时器
    void removeTimerSync(uint32 handle);							// 异步 移除计时器
    void changeTimerSync(uint32 handle, int64 timeCount);			// 异步 更改计时器的时间
    int64 getTimerLeftSync(uint32 handle);							// 异步 获取计时器剩余时间

    uint32 startTimer(uint32 callbackID, int64 timeCount, Handler* pHandler);			// 开始一个计时器
    void removeTimer(uint32 handle);								// 移除计时器
    void changeTimer(uint32 handle, int64 timeCount);				// 更改计时器的时间
    int64 getTimerLeft(uint32 handle);								// 获取计时器剩余时间
public:
	TimerHandle* createTimerHandle(void){
		TimerHandle* pHandle;
		lock();
		pHandle = m_pTimerHandlePool->create();
		unlock();
		return pHandle;
	}
	TimerHandle* getTimerHandle(uint32 handle){
		return m_pTimerHandlePool->get(handle);
	}
	void removeTimerHandle(TimerHandle* pHandle){
		pHandle->clearTimer();
    	pHandle->setActiveObject(0);	// 释放占用
    	lock();
		m_pTimerHandlePool->idle(pHandle);
		unlock();
	}
protected:
	bool initialize(void);
	void destroy(void);
};

class StartTimerTask : public Task
{
public:
	StartTimerTask(void) : Task(), m_timeCount(-1), m_handlerID(0), m_callbackID(0), m_timerHandle(0) {}
	virtual ~StartTimerTask(void);

	// from Task
	virtual void doTask(Handler* pHandler);
	virtual void doWorkerTask(ActiveWorker* pHandler){}

	uint32 getTimerHandle(void) const { return m_timerHandle; }
	void setTimerHandle(uint32 handle){ m_timerHandle = handle; }
	uint32 getCallbackID(void) const { return m_callbackID; }
	void setCallbackID(uint32 id){ m_callbackID = id; }
	int64 getTimeCount(void) const { return m_timeCount; }
	void setTimeCount(int64 timeCount) { m_timeCount = timeCount; }
	void setActiveObject(uint32 handlerID){ m_handlerID = handlerID; }
public:
	int64 m_timeCount;					// 超时的时间
	uint32 m_handlerID;					// 当前Timer回调对象ID
	uint32 m_callbackID;				// 返回的callback ID
	uint32 m_timerHandle;				// TimerHandle的ID
};

class RemoveTimerTask : public Task
{
public:
	RemoveTimerTask(void) : Task(), m_timerHandle(0) {}
	virtual ~RemoveTimerTask(void){}

	// from Task
	virtual void doTask(Handler* pHandler);
	virtual void doWorkerTask(ActiveWorker* pHandler){}

	uint32 getTimerHandle(void) const { return m_timerHandle; }
	void setTimerHandle(uint32 handle){ m_timerHandle = handle; }
public:
	uint32 m_timerHandle;				// TimerHandle的ID
};

class ChangeTimerTask : public Task
{
public:
	ChangeTimerTask(void) : Task(), m_timeCount(-1), m_timerHandle(0) {}
	virtual ~ChangeTimerTask(void){}

	// from Task
	virtual void doTask(Handler* pHandler);
	virtual void doWorkerTask(ActiveWorker* pHandler){}

	uint32 getTimerHandle(void) const { return m_timerHandle; }
	void setTimerHandle(uint32 handle){ m_timerHandle = handle; }
	int64 getTimeCount(void) const { return m_timeCount; }
	void setTimeCount(int64 timeCount) { m_timeCount = timeCount; }
public:
	int64 m_timeCount;					// 超时的时间
	uint32 m_timerHandle;				// TimerHandle的ID
};

NS_HIVE_END

#endif
