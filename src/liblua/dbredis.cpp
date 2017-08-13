//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/12/6
// Time: 下午10:52
// To change this template use File | Settings | File Templates.
//

#include "dbredis.h"

NS_HIVE_BEGIN

DBRedis::DBRedis(void) : Object0824(), m_pContext(NULL) {

}
DBRedis::~DBRedis(void){
    closeRedis();
}
bool DBRedis::connectRedis(const char* ip, uint16 port){
    closeRedis();
    m_pContext = redisConnect(ip, port);
    if (m_pContext != NULL && m_pContext->err) {
        closeRedis();  // 释放掉对象
        return false;
    }
    return true;
}
void DBRedis::closeRedis(void){
    if( m_pContext != NULL ){
        redisFree(m_pContext);
        m_pContext = NULL;
    }
}
bool DBRedis::identifyRedis(const char* pwd){
    redisReply* reply = (redisReply*)redisCommand(m_pContext, "AUTH %s", pwd);
    if( !checkReplyStatus(reply) ){
        freeReplyObject(reply);
        return false;
    }
    if( reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0 ){
        freeReplyObject(reply);
        return true;
    }else{
        freeReplyObject(reply);
        return false;
    }
}
int DBRedis::command(lua_State* tolua_S, bool skipResult, int queryNumber){
	size_t length;
	m_argv.resize(queryNumber, NULL);
	m_argvlen.resize(queryNumber, 0);
    // 从table数组中获取所有的执行字符串
    for(int i=0; i<queryNumber; ++i){
        lua_rawgeti(tolua_S, -1, i+1);
        m_argv[i] = ((char*)lua_tolstring(tolua_S,-1,&length));
        m_argvlen[i] = length;
        lua_pop(tolua_S, 1);
    }
	if(skipResult == true){
		if(!this->executeCommandArgvWithoutReturn(queryNumber, m_argv.data(), m_argvlen.data())){
			return 0;
		}
		lua_pushboolean(tolua_S, true);
		return 1;
	}
	return this->executeCommandArgv(tolua_S, queryNumber, m_argv.data(), m_argvlen.data());
}
int DBRedis::executeCommandArgv(lua_State* tolua_S, int argc, const char **argv, const size_t *argvlen){
    redisReply* reply;
    reply = (redisReply*)redisCommandArgv(m_pContext, argc, argv, argvlen);
    // 状态检测失败，返回错误
    if( !checkReplyStatus(reply) ){
        // 释放掉当前的reply
        freeReplyObject(reply);
        return 0;
    }
    // 解析参数
    checkRedisReplyData(tolua_S, reply);
    // 释放掉当前的reply
    freeReplyObject(reply);
    return 1;
}
bool DBRedis::executeCommandArgvWithoutReturn(int argc, const char **argv, const size_t *argvlen){
    redisReply* reply;
    reply = (redisReply*)redisCommandArgv(m_pContext, argc, argv, argvlen);
    if( !checkReplyStatus(reply) ){
        freeReplyObject(reply);
        return false;
    }
    freeReplyObject(reply);
    return true;
}
bool DBRedis::checkReplyStatus(redisReply* reply){
    if( reply == NULL ){
        // 连接句柄已经出错了
        return false;
    }
    if( reply->type==REDIS_REPLY_ERROR ){
        // 查询出错
        return false;
    }
    return true;
}
// 这个函数只会返回一个值
void DBRedis::checkRedisReplyData(lua_State* tolua_S, redisReply* reply){
    switch( reply->type ){
        case REDIS_REPLY_STRING: {
            lua_pushlstring(tolua_S, reply->str, reply->len);
            break;
        }
        case REDIS_REPLY_INTEGER: {
            lua_pushnumber(tolua_S, (lua_Number)reply->integer);
            break;
        }
        case REDIS_REPLY_ARRAY: {
            // 这里需要循环检测整个array
            lua_createtable(tolua_S, reply->elements, 0);
            for(size_t i=0; i<reply->elements; ++i){
                checkRedisReplyData(tolua_S, reply->element[i]);
                lua_rawseti(tolua_S, -2, i+1);
            }
            break;
        }
        case REDIS_REPLY_STATUS: {
        	lua_pushlstring(tolua_S, reply->str, reply->len);
            break;
        }
        case REDIS_REPLY_NIL: lua_pushnil(tolua_S); break;       // 什么事情都不做
        case REDIS_REPLY_ERROR: lua_pushnil(tolua_S); break;   // 在前面已经检测过了
        default: lua_pushnil(tolua_S); break;
    }
}

NS_HIVE_END
