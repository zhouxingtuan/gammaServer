//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/2
// Time: 上午7:15
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__globalhandler__
#define __hive__globalhandler__

#include "handler.h"
#include "destinationgroup.h"

NS_HIVE_BEGIN

class GlobalHandler : public RefObject, public Sync
{
public:
	typedef DestinationGroup<Handler> HandlerDestinationGroup;
	HandlerDestinationGroup* m_pGroup;
public:
	GlobalHandler(void);
	virtual ~GlobalHandler(void);

	static GlobalHandler* getInstance(void);
	static GlobalHandler* createInstance(void);
	static void destroyInstance(void);

	// 派发任务给某一个Handler
	bool dispatchTask(uint32 handle, Task* pTask);
	// 将消息派发给某一个Handler，这里应该检查nodeID==currentNodeID,service==0；一般不会直接调用
	bool dispatchToHandler(Packet* pPacket);
	// 激活一个Handler的Timer
	int64 activeTimer(uint32 handle, uint32 callbackID);
	// 创建全局的HandlerPool
	bool createPool(uint32 poolType, DestinationCreateFunction create, DestinationDestroyFunction destroy);
	// 创建一个目标Handler
	uint32 createDestination(uint32 poolType, Destination::index_type index);
	bool idleDestination(uint32 handle);
	bool removeDestination(uint32 handle);

	template<class _OBJECT_>
	_OBJECT_* getDestination(uint32 handle){
		Destination* pDes = m_pGroup->getDestination(handle);
		if(NULL == pDes){
			LOG_DEBUG("can not find handle=%d", handle);
			return NULL;
		}
		return static_cast<_OBJECT_*>(pDes);
	}
	HandlerDestinationGroup* getGroup(void) { return m_pGroup; }
	void initialize(void);
	void destroy(void);

};

NS_HIVE_END

#endif
