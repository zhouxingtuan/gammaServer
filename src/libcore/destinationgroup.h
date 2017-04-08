//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/2/26
// Time: 下午2:32
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__destinationgroup__
#define __hive__destinationgroup__

#include "destinationpool.h"
#include "packet.h"
#include "task.h"

NS_HIVE_BEGIN

#define DESTINATION_MAX_GROUP 8

class DestinationGroup : public RefObject
{
public:
	typedef std::vector<DestinationPool*> PoolVector;
protected:
	PoolVector m_pools;		// 对象存储数组
	uint32 m_nodeID;		// 节点ID
	uint32 m_serviceID;		// 服务的ID
public:
	DestinationGroup(uint32 nodeID, uint32 serviceID) : RefObject(), m_nodeID(nodeID), m_serviceID(serviceID) {
		m_pools.resize(DESTINATION_MAX_GROUP, NULL);
	}
	virtual ~DestinationGroup(void){ clear(); }

	bool dispatchPacket(uint32 handle, Packet* pPacket, Task* pTask){
		Destination* pDes = getDestination(handle);
		if(NULL == pDes){
			return false;
		}
		pDes->onReceivePacket(pPacket, pTask);
		return true;
	}
	Destination* getDestination(uint32 handle){
		DestinationHandle h(handle);
		DestinationPool* pPool = getPool(h.getType());
		if(NULL == pPool){
			return NULL;
		}
		return pPool->get(handle);
	}
	Destination* createDestination(uint32 poolType, Destination::index_type index){
		DestinationPool* pPool = getPool(poolType);
		if(NULL == pPool){
			return NULL;
		}
		return pPool->create(index);
	}
	bool idleDestination(uint32 handle){
		DestinationHandle h(handle);
		DestinationPool* pPool = getPool(h.getType());
		if(NULL == pPool){
			return false;
		}
		return pPool->idle(handle);
	}
	bool removeDestination(uint32 handle){
		DestinationHandle h(handle);
		DestinationPool* pPool = getPool(h.getType());
		if(NULL == pPool){
			return false;
		}
		return pPool->remove(handle);
	}
	DestinationPool* getPool(uint32 poolType){
		if(poolType >= (uint32)m_pools.size()){
			return NULL;
		}
		return m_pools[poolType];
	}
	DestinationPool* createPool(uint32 poolType, DestinationCreateFunction create, DestinationDestroyFunction destroy){
		if(poolType >= (uint32)m_pools.size()){
			return NULL;
		}
		DestinationPool* pPool = m_pools[poolType];
		if(NULL == pPool){
			pPool = new DestinationPool();
			pPool->registerFunction(m_nodeID, m_serviceID, poolType, create, destroy);
			m_pools[poolType] = pPool;
		}
		return pPool;
	}
	void clear(void){
		for(auto pPool : m_pools){
			if(NULL != pPool){
				pPool->release();
			}
		}
		m_pools.clear();
	}
};

NS_HIVE_END

#endif
