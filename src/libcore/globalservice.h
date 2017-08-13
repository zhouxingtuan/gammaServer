//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/2
// Time: 上午7:12
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__globalservice__
#define __hive__globalservice__

#include "epollworker.h"
#include "globalhandler.h"

NS_HIVE_BEGIN

#define MAX_EPOLL_WORKER_NUMBER 31

class GlobalService : public RefObject, public Sync
{
public:
	typedef std::vector<EpollWorker*> EpollWorkerVector;
public:
	EpollWorkerVector m_epollWorkers;
	std::atomic<uint32> m_epollMagic;
	uint32 m_epollWorkerNumber;
public:
	GlobalService(void);
	virtual ~GlobalService(void);

	static GlobalService* getInstance(void);
	static GlobalService* createInstance(void);
	static void destroyInstance(void);

	// 发送curl请求
	void sendCurlRequest(RequestData* pRequest);
	// 平均分发请求给相应的EpollWorker
	bool dispatchTaskEqualToEpollWorker(Task* pTask);
	// 发送任务给某一个Service
	bool dispatchTask(uint32 handle, Task* pTask);
	bool dispatchTaskToEpollWorkerHandle(uint32 handle, Task* pTask);
	bool dispatchTaskToEpollWorker(uint32 service, Task* pTask);

	// 从网络收到消息进行分发操作；根据Packet内部记录的数据nodeID,service
	bool dispatchToService(uint32 handle, Packet* pPacket);
	// 发送消息给某一个节点；nodeID查找到网络连接，handle指向对方节点的Handler
	bool sendToNode(uint32 nodeID, uint32 handle, Packet* pPacket);
	// 发送消息给当前节点的某一个连接，或者Handler；handle和packet中的destination可以不一样
	bool sendToService(uint32 handle, Packet* pPacket);
	// 分发消息给网络连接，handle和packet的destination可以不一样
	bool dispatchToEpollWorker(uint32 handle, Packet* pPacket);

	EpollWorker* getWorker(uint32 service){
		if(service >= (uint32)m_epollWorkers.size()){
			return NULL;
		}
		return m_epollWorkers[service];
	}
	void initialize(uint16 epollWorkerNumber);
	void destroy(void);
	uint32 nextEpollMagic(void){
		return (uint32)(m_epollMagic++);
	}
};

NS_HIVE_END

#endif
