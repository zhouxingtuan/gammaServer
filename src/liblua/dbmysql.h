//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/12/6
// Time: 下午10:52
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__dbmysql__
#define __hive__dbmysql__

#include "script.h"
#include <mysql/mysql.h>

NS_HIVE_BEGIN

/*--------------------------------------------------------------------*/

class DBMysql : public Object0824
{
public:
	friend class DBMysqlManager;
public:
    DBMysql(void);
    virtual ~DBMysql(void);

	static DBMysql* createObject(void){
		return new DBMysql();
	}
	static void releaseObject(DBMysql* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	bool connectMySql(const char* ip, const char* user, const char* pwd, const char* db, uint16 port);
	void closeMysql(void);
	bool pingMysql(void);
	int64 getLastAutoID(void);
	void setAutoCommit(bool on);
	bool commit(void);
	bool rollback(void);

	bool execute(const char* query, int queryNumber, bool skipResult){ return false; }
	int execute(lua_State* tolua_S, const char* query, uint32 length, int queryNumber, bool skipResult);

	const char* escape(const char* ptr){ return ptr; }
	int escape(lua_State* tolua_S, const char* ptr, uint32 length);
private:

    int executeQuery(lua_State* tolua_S, const char* query, unsigned int length, int queryNumber);
    bool executeQueryWithoutReturn(const char* query, unsigned int length);
    void parseResult(lua_State* tolua_S, MYSQL_RES* result);
    void parseOther(lua_State* tolua_S, MYSQL_RES* result);
private:
	MYSQL* m_pMysql;
	Buffer* m_pBuffer;
};

typedef ObjectPool<DBMysql> DBMysqlPool;

NS_HIVE_END

#endif
