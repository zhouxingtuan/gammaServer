//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/12/6
// Time: 下午10:52
// To change this template use File | Settings | File Templates.
//

#include "dbmysql.h"

NS_HIVE_BEGIN

DBMysql::DBMysql(void) : Object0824(), m_pMysql(NULL), m_pBuffer(NULL) {

}
DBMysql::~DBMysql(void){
    closeMysql();
}
bool DBMysql::connectMySql(const char* ip, const char* user, const char* pwd, const char* db, uint16 port){
    closeMysql();     // 释放mysql句柄
    m_pBuffer = new Buffer(512);
    m_pBuffer->retain();
    m_pMysql = mysql_init( NULL );
 	if (NULL == mysql_real_connect(m_pMysql, ip, user, pwd, db, port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS)) {
 		closeMysql();     // 释放mysql句柄
 		return false;
 	}
    return true;
}
void DBMysql::closeMysql(void){
    if( m_pMysql != NULL ){
		mysql_close( m_pMysql );
		m_pMysql = NULL;
	}
	SAFE_RELEASE(m_pBuffer)
}
bool DBMysql::pingMysql(void){
    if( mysql_ping(m_pMysql) ){
        return false;
    }
    return true;
}
int64 DBMysql::getLastAutoID(void){
	return (int64)mysql_insert_id(m_pMysql);
}
void DBMysql::setAutoCommit(bool on){
	if (on == true) {
		mysql_autocommit(m_pMysql, 1); /* Set it ON */
	} else {
		mysql_autocommit(m_pMysql, 0);
	}
}
bool DBMysql::commit(void){
	return (!mysql_commit(m_pMysql));
}
bool DBMysql::rollback(void){
	return (!mysql_rollback(m_pMysql));
}
int DBMysql::escape(lua_State* tolua_S, const char* ptr, uint32 length){
    int result;
    m_pBuffer->resize( length*2+2 );
    result = mysql_real_escape_string(m_pMysql, m_pBuffer->data(), ptr, length);
    if( result > 0 ){
        lua_pushlstring(tolua_S, m_pBuffer->data(), result);
        return 1;
    }
    return 0;
}
int DBMysql::execute(lua_State* tolua_S, const char* query, uint32 length, int queryNumber, bool skipResult){
	if( skipResult == true ){
		if( !this->executeQueryWithoutReturn(query, length) ){
			return 0;
		}
		lua_pushboolean(tolua_S, true);
		return 1;
	}
	return this->executeQuery(tolua_S, query, length, queryNumber);
}

int DBMysql::executeQuery(lua_State* tolua_S, const char* query, unsigned int length, int queryNumber){
	if ( mysql_real_query(m_pMysql, query, length) ) {
//        closeMysql();	// 上层自己关闭
		//查询失败，不写入任何数据
		return 0;
	}
    MYSQL_RES* result;
    int index = 1;
	lua_createtable(tolua_S, queryNumber, 0);
	do{
		if( !(result = mysql_store_result(m_pMysql)) ){
		    //查询出Bug或者是非select类型的查询
			parseOther(tolua_S, result);
		}else {
		    //select类型的查询
			parseResult(tolua_S, result);
		}
		lua_rawseti(tolua_S, -2, index++);
		mysql_free_result( result );
	}while( !mysql_next_result(m_pMysql) );
	return 1;
}
bool DBMysql::executeQueryWithoutReturn(const char* query, unsigned int length){
    MYSQL_RES* result;
    if ( mysql_real_query(m_pMysql, query, length) ) {
//        closeMysql();	// 上层自己关闭
        return false;
    }
    // 查询一旦结束，需要调用 mysql_store_result 清空结果集，不然后面的操作会失败
	do{
		result = mysql_store_result(m_pMysql);
		mysql_free_result( result );
	}while( !mysql_next_result(m_pMysql) );
    return true;
}
void DBMysql::parseResult(lua_State* tolua_S, MYSQL_RES* result){
    MYSQL_FIELD* field;
	MYSQL_ROW row;
	unsigned long* row_len;
	unsigned int num_fields;
	unsigned int num_rows;
	std::vector<int> field_types;
	int index = 1;
    //最外层结果的table
    num_rows = mysql_num_rows(result);
    lua_createtable(tolua_S, num_rows+1, 0);

	num_fields = mysql_num_fields(result);
	//column names
	lua_createtable(tolua_S, num_fields, 0);
	for(unsigned int i = 0; i < num_fields; i++) {
		field = mysql_fetch_field_direct(result, i);
		field_types.push_back(field->type);
		lua_pushstring(tolua_S, field->name);
		lua_rawseti(tolua_S, -2, i+1);
	}
	lua_rawseti(tolua_S, -2, index++);

	//row datas
	while ( (row = mysql_fetch_row( result )) ) {
	    row_len = mysql_fetch_lengths( result );
	    lua_createtable(tolua_S, num_fields, 0);
		for(unsigned int i = 0; i < num_fields; i++) {
            //数字全部使用double来表示，其它用string
            if(row[i] != NULL){
                if( IS_NUM(field_types[i]) ){
                    lua_pushnumber(tolua_S, (lua_Number)atof(row[i]));
                }else{
                    lua_pushlstring(tolua_S, row[i], row_len[i]);
                }
            }else{
                //没有值则直接写入nil，Lua层解析已经对它处理
                lua_pushnil(tolua_S);
            }
            lua_rawseti(tolua_S, -2, i+1);
	   	}//end for
	   	lua_rawseti(tolua_S, -2, index++);
	}// end while
	//最外层的table
}
void DBMysql::parseOther(lua_State* tolua_S, MYSQL_RES* result){
    static char affected_rows[] = "affected_rows";
    static char why_come_here[] = "why_come_here";
    unsigned int num_rows;
	//result begin
	if( mysql_errno(m_pMysql) ){
		lua_createtable(tolua_S, 1, 0);
		const char* err = mysql_error(m_pMysql);
		lua_pushstring(tolua_S, err);
		lua_rawseti(tolua_S, -2, 1);
	}else if( mysql_field_count(m_pMysql)==0 ){
		lua_createtable(tolua_S, 2, 0);
		lua_pushlstring(tolua_S, affected_rows, sizeof(affected_rows));
		lua_rawseti(tolua_S, -2, 1);
		num_rows = mysql_affected_rows(m_pMysql);
		lua_pushnumber(tolua_S, (lua_Number)num_rows);
		lua_rawseti(tolua_S, -2, 2);
	}else{
		lua_createtable(tolua_S, 1, 0);
		lua_pushlstring(tolua_S, why_come_here, sizeof(why_come_here));
		lua_rawseti(tolua_S, -2, 1);
	}
}

NS_HIVE_END
