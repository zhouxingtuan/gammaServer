//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/4
// Time: 下午5:20
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__objecthashpool__
#define __hive__objecthashpool__

#include "object.h"

NS_HIVE_BEGIN

template<class _OBJECT_>
class HashPool : public RefObject
{
public:
	typedef std::vector<void*> ObjectVector;
	typedef std::vector<typename _OBJECT_::handle_type> HandleVector;
public:
	HashPool(void) : RefObject(), m_maxHashNumber(MAX_1616_HASH_SIZE) {
		m_objects.resize(1, NULL);	// 下标为0的值不使用
	}
	virtual ~HashPool(void){
		clear();
	}

	uint32 hangup(_OBJECT_* pObj){
		typename _OBJECT_::index_type index;
		struct _OBJECT_::Handle h;
		if( !m_idleHandle.empty() ){
			h = m_idleHandle.back();
			m_idleHandle.pop_back();
			index = h.getIndex();
		}else{
			index = (typename _OBJECT_::index_type)m_objects.size();
			if(index > m_maxHashNumber){
				return 0;
			}
			m_objects.resize(index + 1, NULL);
			h.setIndex(index);
		}
		pObj->setHandle(h.getHandle());
		pObj->retain();
		m_objects[index] = pObj;
		return pObj->getHandle();
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
	bool finish(_OBJECT_* pObj){
		return finish(pObj->getHandle());
	}
	bool finish(typename _OBJECT_::handle_type handle){
		_OBJECT_* pObj = NULL;
		struct _OBJECT_::Handle h(handle);
		typename _OBJECT_::index_type index = h.getIndex();
		if( index < (typename _OBJECT_::index_type)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			m_objects[index] = NULL;
			pObj->release();
			h.increaseVersion();
			m_idleHandle.push_back(h.getHandle());
			return true;
		}
		return false;
	}
	void clear(void){
		for(auto pObj : m_objects){
			((_OBJECT_*)pObj)->release();
		}
		m_objects.clear();
		m_objects.resize(1, NULL);	// 下标为0的值不使用
		m_idleHandle.clear();
	}
	uint32 size(void) const { return (uint32)m_objects.size(); }
	uint32 getMaxHashNumber(void) const { return m_maxHashNumber; }
	void setMaxHashNumber(uint32 number) { m_maxHashNumber = number; }
	uint32 getIdleSize(void) const { return (uint32)m_idleHandle.size(); }
protected:
	ObjectVector m_objects;
	HandleVector m_idleHandle;
	uint32 m_maxHashNumber;
};

// Object系统有自己的内存管理机制，自身管理new/delete

template<class _OBJECT_>
class ObjectHashPool : public RefObject
{
public:
	typedef std::vector<void*> ObjectVector;
public:
	ObjectHashPool(void) : RefObject() {}
	virtual ~ObjectHashPool(void){
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
	_OBJECT_* create(uint32 index){
		if(index >= (uint32)m_objects.size()){
			m_objects.resize(index + 1, NULL);
		}
		_OBJECT_* pObj = (_OBJECT_*)m_objects[index];
		if(NULL == pObj){
			pObj = _OBJECT_::createObject();
			m_objects[index] = pObj;
		}
		return pObj;
	}
	_OBJECT_* get(uint32 index){
		_OBJECT_* pObj = NULL;
		if( index < (uint32)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		return pObj;
	}
	void remove(uint32 index){
		_OBJECT_* pObj = NULL;
		if( index < (uint32)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		if( NULL != pObj ){
			m_objects[index] = NULL;
			_OBJECT_::releaseObject(pObj);
		}
	}
	void clear(void){
		for(auto pObj : m_objects){
			_OBJECT_::releaseObject((_OBJECT_*)pObj);
		}
		m_objects.clear();
	}
	uint32 size(void) const { return (uint32)m_objects.size(); }
	ObjectVector& getObjectVector(void) { return m_objects; }
protected:
	ObjectVector m_objects;
};

template<typename _VALUE_, _VALUE_ _DEFAULT_>
class ValueHashPool : public RefObject
{
public:
	typedef std::vector<_VALUE_> ValueVector;
public:
	ValueHashPool(void) : RefObject() {}
	virtual ~ValueHashPool(void){}

	bool set(uint32 key, _VALUE_ value){
		if(key >= (uint32)m_values.size()){
			m_values.resize(key + 1, _DEFAULT_);
		}
		m_values[key] = value;
		return true;
	}
	_VALUE_ get(uint32 key){
		_VALUE_ value = _DEFAULT_;
		if(key < (uint32)m_values.size()){
			value = m_values[key];
		}
		return value;
	}
	void remove(uint32 key){
		if(key < (uint32)m_values.size()){
			m_values[key] = _DEFAULT_;
		}
	}
	void removeByValue(_VALUE_ value){
		for(uint32 i = 0; i < (uint32)m_values.size(); ++i){
			if(m_values[i] == value){
				m_values[i] = _DEFAULT_;
			}
		}
	}
	uint32 size(void) {
		return (uint32)m_values.size();
	}
	void clear(void){
		m_values.clear();
	}
	ValueVector& getValueVector(void) { return m_values; }
protected:
	ValueVector m_values;
};

#define DEFINE_MAP_POOL(_NAME_, _KEY_, _OBJECT_)\
class _NAME_ : public RefObject\
{\
public:\
	typedef std::map<_KEY_, _OBJECT_*> ObjectMap;\
public:\
	_NAME_(void) : RefObject() {}\
	virtual ~_NAME_(void){\
		clear();\
	}\
	_OBJECT_* create(_KEY_ key){\
		ObjectMap::iterator itCur = m_objects.find(key);\
		if(itCur != m_objects.end()){\
			return (itCur->second);\
		}\
		_OBJECT_* pObj = _OBJECT_::createObject();\
		m_objects.insert(std::make_pair(key, pObj));\
		return pObj;\
	}\
	_OBJECT_* get(_KEY_ key){\
		ObjectMap::iterator itCur = m_objects.find(key);\
		if(itCur != m_objects.end()){\
			return (itCur->second);\
		}\
		return NULL;\
	}\
	void remove(_KEY_ key){\
		ObjectMap::iterator itCur = m_objects.find(key);\
		if(itCur != m_objects.end()){\
			_OBJECT_::releaseObject(itCur->second);\
			m_objects.erase(key);\
		}\
	}\
	uint32 size(void) {\
		return (uint32)m_objects.size();\
	}\
	void clear(void){\
		for(auto &kv : m_objects){\
			_OBJECT_::releaseObject(kv.second);\
		}\
		m_objects.clear();\
	}\
	uint32 getObjectSize(void) const { return (uint32)m_objects.size(); }\
	ObjectMap& getObjectMap(void) { return m_objects; }\
protected:\
	ObjectMap m_objects;\
};

NS_HIVE_END

#endif
