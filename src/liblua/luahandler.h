//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/13
// Time: 下午9:13
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__luahandler__
#define __hive__luahandler__

#include "core.h"
#include "script.h"
#include "md5.h"
#include "dbmysql.h"
#include "dbredis.h"

NS_HIVE_BEGIN

#define DEFINE_CREATE_REMOVE(_POOL_, _OBJECT_)\
	_OBJECT_* create##_OBJECT_(void){ return _POOL_->create(); }\
	bool remove##_OBJECT_(_OBJECT_* pObj){ return this->remove##_OBJECT_(pObj->getHandle()); }\
	bool remove##_OBJECT_(uint32 handle){\
		_OBJECT_* pObj;\
		pObj = _POOL_->get(handle);\
		if(NULL != pObj){\
			pObj->closeDB();\
			_POOL_->idle(handle);\
			return true;\
		}\
		return false;\
	}

class LuaHandler : public Handler
{
public:
	Script* m_pScript;
	DBRedisPool* m_pDBRedisPool;
	DBMysqlPool* m_pDBMysqlPool;
	MD5Pool* m_pMD5Pool;
public:
	LuaHandler(void);
	virtual ~LuaHandler(void);

	// from Destination
	virtual void onReceivePacket(Packet* pPacket, Task* pTask);

	// from Handler
	virtual void onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK);
	virtual void onOpenClientOK(uint32 clientHandle, OpenClientOKTask* pTask);
	virtual void onOpenClient(uint32 callbackID, uint32 clientHandle, OpenClientTask* pTask);
	virtual void onOpenSocketListener(uint32 callbackID, uint32 listenerHandle, OpenSocketListenerTask* pTask);
	virtual void onOpenHttpListener(uint32 callbackID, uint32 listenerHandle, OpenHttpListenerTask* pTask);
	virtual void onOpenHttpsListener(uint32 callbackID, uint32 listenerHandle, OpenHttpsListenerTask* pTask);
	virtual void onCloseListener(uint32 callbackID, uint32 listenerHandle, CloseListenerTask* pTask);
	virtual void onCloseConnect(uint32 callbackID, uint32 connectHandle, CloseConnectTask* pTask);
	virtual void onInitialize(const std::string& param);
	virtual void onDestroy(void);
	virtual int64 onTimerUpdate(uint32 callbackID);

	DBRedis* createRedis(void){ return m_pDBRedisPool->create(); }
	bool removeRedis(DBRedis* pDBRedis){ return this->removeRedis(pDBRedis->getHandle()); }
	bool removeRedis(uint32 handle){
		DBRedis* pDBRedis;
		pDBRedis = m_pDBRedisPool->get(handle);
		if(NULL != pDBRedis){
			pDBRedis->closeRedis();
			m_pDBRedisPool->idle(handle);
			return true;
		}
		return false;
	}

	DBMysql* createMysql(void){ return m_pDBMysqlPool->create(); }
	bool removeMysql(DBMysql* pDBMysql){ return this->removeMysql(pDBMysql->getHandle()); }
	bool removeMysql(uint32 handle){
		DBMysql* pDBMysql;
		pDBMysql = m_pDBMysqlPool->get(handle);
		if(NULL != pDBMysql){
			pDBMysql->closeMysql();
			m_pDBMysqlPool->idle(handle);
			return true;
		}
		return false;
	}

	MD5* createMD5(void){ return m_pMD5Pool->create(); }
	bool removeMD5(MD5* pMD5){ return this->removeMD5(pMD5->getHandle()); }
	bool removeMD5(uint32 handle){
		MD5* pMD5;
		pMD5 = m_pMD5Pool->get(handle);
		if(NULL != pMD5){
			m_pMD5Pool->idle(handle);
			return true;
		}
		return false;
	}

	void initializeHandler(void);
	void destroyHandler(void);
};

NS_HIVE_END

#endif
