//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/10/12
// Time: 下午9:13
// To change this template use File | Settings | File Templates.
//

#include "timer.h"

NS_HIVE_BEGIN

bool TimerObject::setTimer(int64 timeCount, Timer* pTimer){
//	if(NULL != m_pTimerList && NULL != m_pTimerList->m_pTimer){
//		return m_pTimerList->m_pTimer->timerAdd(this, timeCount);
//	}
//	return false;
	return pTimer->timerAdd(this, timeCount);
}
bool TimerObject::clearTimer(void){
	if(NULL != m_pTimerList){
		m_pTimerList->remove(this);
		return true;
	}
	return false;
}

void TimerList::update(int64 current_time, TimerObjectVector& objects){
	TimerObject* pTemp;
	TimerObject* pObject = m_pHead;
	while( NULL != pObject && current_time >= pObject->m_condition ){
		pTemp = pObject;
		pObject = pObject->m_pNext;	// 检查下一个节点

		objects.push_back(std::make_pair(pTemp, this));
		pTemp->m_pTimerList = NULL;
		pTemp->m_pPre = NULL;
		pTemp->m_pNext = NULL;
	};
	m_pHead = pObject;	// 之前的节点全部出队列
	if(NULL == m_pHead){
		m_pTail = NULL;
	}else{
		m_pHead->m_pPre = NULL;
	}
}
void TimerList::add(TimerObject* pObject){
	this->remove(pObject);		// 移除出旧的列表
	pObject->m_index = m_pTimer->getTimerIndex();
	pObject->m_condition = get_next_time_ms(m_timeCount);
	pObject->m_pTimerList = this;
	pObject->m_pPre = NULL;
	pObject->m_pNext = NULL;
	if( NULL == m_pHead ){
		m_pHead = pObject;
		m_pTail = pObject;
		return;
	}
	m_pTail->m_pNext = pObject;
	pObject->m_pPre = m_pTail;
	m_pTail = pObject;
}
void TimerList::remove(TimerObject* pObject){
	TimerList* pTimerList = pObject->m_pTimerList;
	if(NULL == pTimerList){
		return;
	}
	TimerObject* pPre = pObject->m_pPre;
	TimerObject* pNext = pObject->m_pNext;
	if(pObject == pTimerList->m_pHead){
		if(pObject == pTimerList->m_pTail){
			pTimerList->m_pHead = NULL;
			pTimerList->m_pTail = NULL;
		}else{
			pTimerList->m_pHead = pNext;
			pNext->m_pPre = NULL;
		}
	}else{
		if(pObject == pTimerList->m_pTail){
			pTimerList->m_pTail = pPre;
			pPre->m_pNext = NULL;
		}else{
			pPre->m_pNext = pNext;
			pNext->m_pPre = pPre;
		}
	}
	// 清理数据
	pObject->m_pTimerList = NULL;
	pObject->m_pPre = NULL;
	pObject->m_pNext = NULL;
}

Timer::Timer() : RefObject(), m_pPool(NULL), m_timerIndex(0), m_waitTimeout(MAX_INT64) {

}
Timer::~Timer(){
	SAFE_RELEASE(m_pPool)
}
bool Timer::start(uint32 poolSize){
	if(NULL == m_pPool){
		m_pPool = new TimerListPool();
		m_pPool->retain();
	}
	m_pPool->setMaxHashNumber(poolSize);
	fprintf(stderr,"--Timer::start poolSize＝%d time=%d\n", poolSize, (int)time(NULL));
	return true;
}
bool object_sort_function(const TimerObjectListPair& a, const TimerObjectListPair& b){
	return a.first->getIndex() < b.first->getIndex();
}
void Timer::update(void){
	int64 current_time = get_time_ms();
	// 还没有到更新时间，这里不做更新操作
	if(m_waitTimeout > current_time){
		return;
	}
	m_waitTimeout = MAX_INT64;
	TimerObjectVector objects;
	for(auto &pair : m_timerListMap ){
//		fprintf(stderr, "update check timeCount=%lld\n", pair.second->getTimeCount());
		pair.second->update(current_time, objects);
		this->recordWaitTimeout(pair.second->getHeadCondition());
	}
	if(objects.size() > 0){
		std::sort(objects.begin(), objects.end(), object_sort_function);
		TimerObject* pObject;
		TimerList* pTimerList;
		int64 timeCount;
		for(auto &objListPair : objects){
			pObject = objListPair.first;
			pTimerList = objListPair.second;

			timeCount = pObject->timerCallback();

			if(timeCount >= 0 && !pObject->isActive()){
				if(pTimerList->getTimeCount() == timeCount){
					pTimerList->add(pObject);
					this->recordWaitTimeout(pObject->m_condition);
				}else{
					this->timerAdd(pObject, timeCount);
				}
			}
		}
		checkRemoveList();
//		fprintf(stderr, "m_waitTimeout - get_time_ms() = %lld\n", m_waitTimeout - get_time_ms());
	}
}
void Timer::checkRemoveList(void){
	TimerList* pTimerList;
	TimerListMap::iterator itCur = m_timerListMap.begin();
	for(;itCur != m_timerListMap.end();){
		pTimerList = itCur->second;
		if( pTimerList->isEmpty() ){
			pTimerList->clearList();
			m_pPool->idle(pTimerList);
			m_timerListMap.erase(itCur++);
		}else{
			++itCur;
		}
	}
}
bool Timer::timerAdd(TimerObject* pObject, int64 timeCount){
	// 是否添加到旧的列表
	TimerList* pTimerList = pObject->m_pTimerList;
	if( NULL != pTimerList ){
		// 判断只有在当前列表下，才可以直接添加
		if( this == pTimerList->m_pTimer && pTimerList->getTimeCount() == timeCount ){
			pTimerList->add(pObject);
			this->recordWaitTimeout(pObject->m_condition);
			return true;
		}
		// 不添加就从旧的链表删除
		pTimerList->remove(pObject);
	}
	// 找到timeCount对应的list，存在就直接添加到list中
	pTimerList = findTimerList(timeCount);
	if( NULL != pTimerList ){
		pTimerList->add(pObject);
		this->recordWaitTimeout(pObject->m_condition);
		return true;
	}
	// 不存在就获取新的list添加进去
	pTimerList = m_pPool->create();
	if( NULL == pTimerList ){
		return false;
	}
	pTimerList->setTimer(this);
	pTimerList->setTimeCount(timeCount);
	pTimerList->add(pObject);
	m_timerListMap.insert(std::make_pair(timeCount, pTimerList));
	this->recordWaitTimeout(pObject->m_condition);
	return true;
}

NS_HIVE_END
