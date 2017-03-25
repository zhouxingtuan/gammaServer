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

#define DEFAULT_MAX_DESTINATION_POOL 65535

class DestinationPool : public RefObject
{
public:
	typedef std::vector<Destination*> ObjectVector;
//	typedef std::deque<Destination::index_type> IndexQueue;
	typedef std::deque<uint16> IndexQueue;
protected:
	DestinationCreateFunction m_createFunction;
	DestinationDestroyFunction m_destroyFunction;
	ObjectVector m_objects;
	ObjectVector m_idleObjects;
	IndexQueue m_idleSlot;
//	uint32 m_useCount;
	uint32 m_maxHashNumber;
//	uint32 m_slotIndex;
	uint32 m_nodeID;
	uint32 m_poolType;
	uint32 m_serviceID;
public:
	DestinationPool(void) : RefObject(), m_createFunction(NULL), m_destroyFunction(NULL),
//	 	m_useCount(0),
	 	m_maxHashNumber(DEFAULT_MAX_DESTINATION_POOL),
//	 	m_slotIndex(1),
	 	m_nodeID(0), m_poolType(0), m_serviceID(0) {
		m_objects.resize(65536, NULL);	// 下标为0的值不使用
		for(uint32 i=1; i<65536; ++i){
			m_idleSlot.push_back(i);
		}
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
	inline Destination::index_type getNextSlot(void){
//		uint32 index = moveSlot();
//		while(NULL != m_objects[index]){
//			index = moveSlot();
//		};
//		return index;
		if(m_idleSlot.empty()){
			return 0;
		}
		Destination::index_type index = m_idleSlot.front();
		m_idleSlot.pop_front();
		return index;
	}
//	inline uint32 moveSlot(void){
//		if(m_slotIndex > 65535){
//			m_slotIndex = 2;
//			return 1;
//		}
//		return m_slotIndex++;
//	}
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
//		if(m_useCount >= m_maxHashNumber){
//			return NULL;
//		}
		Destination::index_type index = getNextSlot();
		if(index == 0){
			return NULL;
		}
		Destination* pObj = getIdleObject();
		pObj->setService(m_serviceID);
		pObj->setType(m_poolType);
		pObj->setNode(m_nodeID);
		pObj->setIndex(index);
		m_objects[index] = pObj;
//		++m_useCount;
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
			m_idleSlot.push_back(index);
//			--m_useCount;
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
			m_idleSlot.push_back(index);
//			--m_useCount;
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
		m_idleSlot.clear();
		for(uint32 i=1; i<65536; ++i){
			m_idleSlot.push_back(i);
		}
	}
	uint32 size(void) const { return (uint32)(65535 - m_idleSlot.size()); }
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
