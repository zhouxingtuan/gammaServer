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

typedef Destination* (*DestinationCreateFunction)(void);
typedef void (*DestinationDestroyFunction)(Destination* pDes);

#define DEFAULT_MAX_DESTINATION_POOL 10000

class DestinationPool : public RefObject
{
public:
	typedef std::vector<Destination*> ObjectVector;
	typedef std::vector<Destination::handle_type> HandleVector;
protected:
	DestinationCreateFunction m_createFunction;
	DestinationDestroyFunction m_destroyFunction;
	ObjectVector m_objects;
	ObjectVector m_idleObjects;
	uint32 m_useCount;
	uint32 m_maxHashNumber;
	uint32 m_slotIndex;
	uint8 m_nodeID;
	uint8 m_poolType;
	uint8 m_serviceID;
public:
	DestinationPool(void) : RefObject(), m_createFunction(NULL), m_destroyFunction(NULL),
	 	m_useCount(0), m_maxHashNumber(DEFAULT_MAX_DESTINATION_POOL), m_slotIndex(1),
	 	m_nodeID(0), m_poolType(0), m_serviceID(0) {
		m_objects.resize(65536, NULL);	// 下标为0的值不使用
	}
	virtual ~DestinationPool(void){
		clear();
	}
	void registerFunction(uint32 nodeID, uint32 serviceID, uint32 poolType, DestinationCreateFunction create, DestinationDestroyFunction destroy){
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
	Destination* getIdleObject(void){
		Destination* pObj;
		if(!m_idleObjects.empty()){
			pObj = m_idleObjects.back();
			m_idleObjects.pop_back();
		}else{
			pObj = m_createFunction();
		}
		return pObj;
	}
	// 当超出最大缓存数值的时候，会返回NULL
	Destination* create(void){
		if(m_useCount >= m_maxHashNumber){
			return NULL;
		}
		Destination* pObj = getIdleObject();
		uint32 index = getNextSlot();
		pObj->setService(m_serviceID);
		pObj->setType(m_poolType);
		pObj->setNode(m_nodeID);
		pObj->setIndex(index);
		m_objects[index] = pObj;
		++m_useCount;
		return pObj;
	}
	Destination* get(Destination::handle_type handle){
		Destination* pObj = NULL;
		struct DestinationHandle h(handle);
		Destination::index_type index = h.getIndex();
		if( index < (Destination::index_type)m_objects.size() ){
			pObj = m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			return pObj;
		}
		return NULL;
	}
	bool idle(Destination* pObj){
		return idle(pObj->getHandle());
	}
	bool idle(Destination::handle_type handle){
		Destination* pObj = NULL;
		struct DestinationHandle h(handle);
		Destination::index_type index = h.getIndex();
		if( index < (Destination::index_type)m_objects.size() ){
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
	bool remove(Destination* pObj){
		return remove(pObj->getHandle());
	}
	bool remove(Destination::handle_type handle){
		Destination* pObj = NULL;
		struct DestinationHandle h(handle);
		Destination::index_type index = h.getIndex();
		if( index < (Destination::index_type)m_objects.size() ){
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
		m_objects.resize(65536, NULL);	// 下标为0的值不使用
		for(auto pObj : m_idleObjects){
			if(NULL != pObj){
				m_destroyFunction(pObj);
			}
		}
		m_idleObjects.clear();
	}
	uint32 size(void) const { return m_useCount; }
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
