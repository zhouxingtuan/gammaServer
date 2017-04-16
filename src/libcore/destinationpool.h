//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/2/26
// Time: 下午2:23
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__destinationpool__
#define __hive__destinationpool__

#include "destination.h"

NS_HIVE_BEGIN

#define DEFAULT_MAX_DESTINATION_POOL 65535
template<class _OBJECT_>
class DestinationPool : public RefObject
{
public:
	typedef _OBJECT_* (*CreateFunction)(uint32 index);
	typedef void (*DestroyFunction)(_OBJECT_* pDes);
	typedef std::vector<_OBJECT_*> ObjectVector;
protected:
	CreateFunction m_createFunction;
	DestroyFunction m_destroyFunction;
	ObjectVector m_objects;
	ObjectVector m_idleObjects;
	uint32 m_useCount;
	uint32 m_slotIndex;
	uint32 m_maxHashNumber;
	uint32 m_nodeID;
	uint32 m_poolType;
	uint32 m_serviceID;
public:
	DestinationPool(void) : RefObject(), m_createFunction(NULL), m_destroyFunction(NULL),
	 	m_useCount(0),
	 	m_slotIndex(1),
	 	m_maxHashNumber(DEFAULT_MAX_DESTINATION_POOL),
	 	m_nodeID(0), m_poolType(0), m_serviceID(0)
	{
		m_objects.resize(65536, NULL);	// 下标为0的值不使用
	}
	virtual ~DestinationPool(void){
		clear();
	}
	void registerFunction(uint32 nodeID, uint32 serviceID, uint32 poolType,
		CreateFunction create, DestroyFunction destroy){
		m_nodeID = nodeID;
		m_serviceID = serviceID;
		m_poolType = poolType;
		m_createFunction = create;
		m_destroyFunction = destroy;
	}
	inline uint32 getNextSlot(void){
		uint32 index = moveSlot();
		while(NULL != m_objects[index]){
			index = moveSlot();
		};
		return index;
	}
	inline uint32 moveSlot(void){
		if(m_slotIndex > 65535){
			m_slotIndex = 2;
			return 1;
		}
		return m_slotIndex++;
	}
	_OBJECT_* getIdleObject(void){
		_OBJECT_* pObj;
		if(!m_idleObjects.empty()){
			pObj = m_idleObjects.back();
			m_idleObjects.pop_back();
		}else{
			pObj = m_createFunction(0);
		}
		return pObj;
	}
	// 当超出最大缓存数值的时候，会返回NULL
	_OBJECT_* create(uint32 index){
		if(m_useCount >= m_maxHashNumber || index >= m_maxHashNumber){
			return NULL;
		}
		_OBJECT_* pObj;
		if(index == 0){
			index = getNextSlot();
			pObj = getIdleObject();
		}else{
			pObj = m_createFunction(index);
		}
		pObj->setService(m_serviceID);
		pObj->setType(m_poolType);
		pObj->setNode(m_nodeID);
		pObj->setIndex(index);
		// 释放掉老的对象
		_OBJECT_* pOldObj = m_objects[index];
		if(NULL != pOldObj){
			m_objects[index] = pObj;
			m_destroyFunction(pOldObj);
		}else{
			m_objects[index] = pObj;
			++m_useCount;
		}
		return pObj;
	}
	_OBJECT_* get(uint32 handle){
		_OBJECT_* pObj = NULL;
		struct DestinationHandle h(handle);
		uint32 index = h.getIndex();
		if( index < (uint32)m_objects.size() ){
			pObj = m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			return pObj;
		}
		return NULL;
	}
	bool idle(_OBJECT_* pObj){
		return idle(pObj->getHandle());
	}
	bool idle(uint32 handle){
		_OBJECT_* pObj = NULL;
		struct DestinationHandle h(handle);
		uint32 index = h.getIndex();
		if( index < (uint32)m_objects.size() ){
			pObj = m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			m_objects[index] = NULL;
			--m_useCount;
			m_idleObjects.push_back(pObj);
			return true;
		}
		return false;
	}
	bool remove(_OBJECT_* pObj){
		return remove(pObj->getHandle());
	}
	bool remove(uint32 handle){
		_OBJECT_* pObj = NULL;
		struct DestinationHandle h(handle);
		uint32 index = h.getIndex();
		if( index < (uint32)m_objects.size() ){
			pObj = m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			m_objects[index] = NULL;
			--m_useCount;
			m_destroyFunction(pObj);
			return true;
		}
		return false;
	}
	void clear(void){
		for(auto pObj : m_objects){
			if(NULL != pObj){
				m_destroyFunction(pObj);
			}
		}
		m_objects.clear();
		for(auto pObj : m_idleObjects){
			if(NULL != pObj){
				m_destroyFunction(pObj);
			}
		}
		m_idleObjects.clear();
		m_objects.resize(65536, NULL);	// 下标为0的值不使用
		m_useCount = 0;
		m_slotIndex = 1;
	}
	uint32 size(void) const { return (uint32)(65535 - m_useCount); }
	uint32 getMaxHashNumber(void) const { return m_maxHashNumber; }
	void setMaxHashNumber(uint32 number) {
		if(number > 65535){
			m_maxHashNumber = 65535;
		}else{
			m_maxHashNumber = number;
		}
	}
	uint32 getIdleSize(void) const { return (uint32)m_idleObjects.size(); }
	ObjectVector& getObjectVector(void) { return m_objects; }
};

NS_HIVE_END

#endif
