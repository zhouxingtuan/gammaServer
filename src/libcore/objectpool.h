//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/4
// Time: 下午1:31
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__obectpool__
#define __hive__obectpool__

#include "object.h"

NS_HIVE_BEGIN

// Object系统有自己的内存管理机制，自身管理new/delete

#define DEFAULT_MAX_OBJECT_POOL MAX_1616_HASH_SIZE

template<class _OBJECT_>
class ObjectPool : public RefObject
{
public:
	typedef std::vector<void*> ObjectVector;
	typedef std::vector<typename _OBJECT_::handle_type> HandleVector;
public:
	ObjectPool(void) : RefObject(), m_maxHashNumber(DEFAULT_MAX_OBJECT_POOL) {
		m_objects.resize(1, NULL);	// 下标为0的值不使用
	}
	virtual ~ObjectPool(void){
		clear();
	}

//	static _OBJECT_* createObject(void){
//		return new _OBJECT_();
//	}
//	static void releaseObject(_OBJECT_* pObj){
//		if(NULL != pObj){
//			delete pObj;
//		}
//	}
	// 当超出最大缓存数值的时候，会返回NULL
	_OBJECT_* create(void){
		typename _OBJECT_::index_type index;
		struct _OBJECT_::Handle h;
		_OBJECT_* pObj;
		if( !m_idleHandle.empty() ){
			h = m_idleHandle.back();
			m_idleHandle.pop_back();
			index = h.getIndex();
			pObj = (_OBJECT_*)m_objects[index];
			if( NULL == pObj ){
				pObj = _OBJECT_::createObject();
				pObj->setHandle(h.getHandle());
				m_objects[index] = pObj;
			}
		}else{
			index = (typename _OBJECT_::index_type)m_objects.size();
			if(index > m_maxHashNumber){
				return NULL;
			}
			pObj = _OBJECT_::createObject();
			pObj->setIndex(index);
			m_objects.push_back(pObj);
		}
		return pObj;
	}
	_OBJECT_* get(typename _OBJECT_::handle_type handle){
		_OBJECT_* pObj = NULL;
		struct _OBJECT_::Handle h(handle);
		typename _OBJECT_::index_type index = h.getIndex();
		if( index < (typename _OBJECT_::index_type)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			return pObj;
		}
		return NULL;
	}
	bool idle(_OBJECT_* pObj){
		return idle(pObj->getHandle());
	}
	bool idle(typename _OBJECT_::handle_type handle){
		_OBJECT_* pObj = NULL;
		struct _OBJECT_::Handle h(handle);
		typename _OBJECT_::index_type index = h.getIndex();
		if( index < (typename _OBJECT_::index_type)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			pObj->increaseVersion();
			m_idleHandle.push_back(pObj->getHandle());
			return true;
		}
		return false;
	}
	bool remove(_OBJECT_* pObj){
		return remove(pObj->getHandle());
	}
	bool remove(typename _OBJECT_::handle_type handle){
		_OBJECT_* pObj = NULL;
		struct _OBJECT_::Handle h(handle);
		typename _OBJECT_::index_type index = h.getIndex();
		if( index < (typename _OBJECT_::index_type)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			m_objects[index] = NULL;
			_OBJECT_::releaseObject(pObj);
			h.increaseVersion();
			m_idleHandle.push_back(h.getHandle());
			return true;
		}
		return false;
	}
	void clear(void){
		for(auto pObj : m_objects){
			_OBJECT_::releaseObject((_OBJECT_*)pObj);
		}
		m_objects.clear();
		m_objects.resize(1, NULL);	// 下标为0的值不使用
		m_idleHandle.clear();
	}
	uint32 size(void) const { return (uint32)m_objects.size(); }
	uint32 getMaxHashNumber(void) const { return m_maxHashNumber; }
	void setMaxHashNumber(uint32 number) { m_maxHashNumber = number; }
	uint32 getIdleSize(void) const { return (uint32)m_idleHandle.size(); }
	ObjectVector& getObjectVector(void) { return m_objects; }
protected:
	ObjectVector m_objects;
	HandleVector m_idleHandle;
	uint32 m_maxHashNumber;
};

NS_HIVE_END

#endif
