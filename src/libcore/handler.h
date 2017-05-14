//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/7
// Time: 下午9:30
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__handler__
#define __hive__handler__

#include "destination.h"
#include "task.h"
#include "packet.h"
#include "timer.h"

NS_HIVE_BEGIN

class HandlerQueue;
class Handler;

#define HANDLER_TIMER_IDLE 		0		// 空闲
#define HANDLER_TIMER_ADD 		1		// 添加
#define HANDLER_TIMER_REMOVE	2		// 删除
#define HANDLER_TIMER_ACTIVE	3		// 激活
#define HANDLER_TIMER_WAIT		4		// 等待

class OpenSocketListenerTask;
class OpenHttpListenerTask;
class OpenHttpsListenerTask;
class CloseListenerTask;
class OpenClientTask;
class OpenClientOKTask;
class CloseConnectTask;
class RequestData;

class Handler : public Destination, public RefObject, public Sync
{
public:
	friend class Worker;
	friend class HandlerQueue;
	friend class Task;
	typedef std::deque<Task*> TaskQueue;
public:
	TaskQueue m_taskQueue;		// 当前主体对象需要完成的任务队列，确保任务独占主体的资源
	bool m_isInHandlerQueue;	// 标记当前主体对象是否已经在队列中等待Worker处理
public:
	Handler(void);
	virtual ~Handler(void);

	// from Destination
	virtual void onReceivePacket(Packet* pPacket, Task* pTask){};			// 处理消息，Destination已经定义

	virtual void onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK) = 0;
	virtual void onOpenClientOK(uint32 clientHandle, OpenClientOKTask* pTask) = 0;
	virtual void onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask) = 0;
	virtual void onOpenSocketListener(uint32 callbackID, uint32 listenerHandle, OpenSocketListenerTask* pTask) = 0;
	virtual void onOpenHttpListener(uint32 callbackID, uint32 listenerHandle, OpenHttpListenerTask* pTask) = 0;
	virtual void onOpenHttpsListener(uint32 callbackID, uint32 listenerHandle, OpenHttpsListenerTask* pTask) = 0;
	virtual void onCloseListener(uint32 callbackID, uint32 listenerHandle, CloseListenerTask* pTask) = 0;
	virtual void onCloseConnect(uint32 callbackID, uint32 connectHandle, CloseConnectTask* pTask) = 0;
	virtual void onInitialize(void) = 0;
	virtual void onDestroy(void) = 0;

	void identifyHive(uint32 connectHandle);
	void openClient(uint32 callbackID, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex);
	void openHttpListener(uint32 callbackID, const char* ip, uint16 port);
	void openHttpsListener(uint32 callbackID, const char* ip, uint16 port);
	void openSocketListener(uint32 callbackID, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex);
	void closeListener(uint32 callbackID, uint32 listenerHandle);
	void closeConnect(uint32 callbackID, uint32 connectHandle);
	void bindAccept(uint32 acceptHandle, uint32 bindHandle);
	void sendCurlRequest(RequestData* pRequest);

	bool receivePacket(Packet* pPacket);							        // 收到一个Packet
    void acceptTask(Task* pTask); 									        // Task 调用接收任务
    void acceptTaskFront(Task* pTask);								        // 任务放在最前面

	virtual int64 onTimerUpdate(uint32 callbackID) = 0;						// 更新定时器
	int64 timerActiveCallback(uint32 callbackID);					        // 异步调用激活Handler的Timer
    uint32 startTimer(uint32 callbackID, int64 timeCount);			        // 开始一个计时器
    void removeTimer(uint32 handle);								        // 移除计时器
    void changeTimer(uint32 handle, int64 timeCount);				        // 更改计时器的时间
    int64 leftTimer(uint32 handle);									        // 获取计时器剩余时间

protected:
	void releaseTask(void);									                // 放弃掉所有任务
	void doTask(void);										                // 执行任务内容
    bool isFinished(void){ return m_taskQueue.empty(); }	                // 是否已经结束
    void doHandler(HandlerQueue* pQueue=NULL);				                // Worker 调用执行任务检测
    void notify(void);						                                // 通知当前handler进入队列
};// end class Handler

class ReceivePacketTask : public Task
{
public:
	ReceivePacketTask(void) : Task(), m_pPacket(NULL) {}
	virtual ~ReceivePacketTask(void){
		SAFE_RELEASE(m_pPacket)
	}

	virtual void doHandlerTask(Handler* pHandler){
		LOG_DEBUG("--ReceivePacketTask::doHandlerTask");
    	pHandler->onReceivePacket(m_pPacket, this);
	}
	virtual void doActiveTask(ActiveWorker* pHandler){}
	void setPacket(Packet* pPacket){
		SAFE_RETAIN(pPacket)
		SAFE_RELEASE(m_pPacket)
		m_pPacket = pPacket;
	}
public:
	Packet* m_pPacket;				// 请求request的packet
};

class TimerUpdateTask : public Task
{
public:
	TimerUpdateTask(void) : m_callbackID(0){}
	virtual ~TimerUpdateTask(void){}

	// from Task
	virtual void doHandlerTask(Handler* pHandler){
		pHandler->onTimerUpdate(m_callbackID);
	}
	virtual void doActiveTask(ActiveWorker* pHandler){}

	inline uint32 getCallbackID(void) const { return m_callbackID; }
	inline void setCallbackID(uint32 id){ m_callbackID = id; }
protected:
	uint32 m_callbackID;				// 返回的callback ID
};

NS_HIVE_END

#endif
