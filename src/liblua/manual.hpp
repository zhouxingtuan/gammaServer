//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/14
// Time: 上午11:29
// To change this template use File | Settings | File Templates.
//

#include "core.h"

#include "lua.hpp"
#include "tolua++.h"

#include "luahandler.h"
#include "md5.h"
#include "dbmysql.h"
#include "dbredis.h"

USING_NS_HIVE

#define TOLUA_RELEASE 1

#undef tolua_pushcppstring
#define tolua_pushcppstring(x,y) lua_pushlstring(x,y.c_str(),(size_t)y.length())

/* method: sendCurlRequest of class  Handler */
static int tolua_liblua_Handler_sendCurlRequest(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_istable(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  size_t body_length = 0;
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* method = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* url = ((const char*)  tolua_tostring(tolua_S,3,0));
  const char* body = ((const char*) lua_tolstring(tolua_S,5,&body_length));
  uint32 callbackID = ((uint32)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendCurlRequest'", NULL);
#endif
  {
  	uint32 handle = self->getHandle();
  	RequestData* pRequest = new RequestData(method, url, body, body_length, handle, callbackID);
  	pRequest->retain();
	/* table 放在索引 't' 处 */
	lua_pushnil(tolua_S);  /* 第一个 key */
	while (lua_next(tolua_S, 4) != 0) {
	 /* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
//	 printf("%s - %s\n", lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)));
	 const char* field = ((const char*)  tolua_tostring(tolua_S,-2,0));
	 const char* value = ((const char*)  tolua_tostring(tolua_S,-1,0));
	 pRequest->appendHeader(field, value);
	 /* 移除 'value' ；保留 'key' 做下一次迭代 */
	 lua_pop(tolua_S, 1);
	}
	self->sendCurlRequest(pRequest);
   	pRequest->release();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendCurlRequest'.",&tolua_err);
 return 0;
#endif
}

static void extendHandler(lua_State* L)
{
    lua_pushstring(L, "Handler");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "sendCurlRequest", tolua_liblua_Handler_sendCurlRequest);
    }
    lua_pop(L, 1);
}

/* method: command of class  DBRedis */
static int tolua_liblua_DBRedis_command(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBRedis",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_istable(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBRedis* self = (DBRedis*)  tolua_tousertype(tolua_S,1,0);
  bool skipResult = ((bool)  tolua_toboolean(tolua_S,2,0));
  int queryNumber = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'command'", NULL);
#endif
  {
   return self->command(tolua_S, skipResult, queryNumber);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'command'.",&tolua_err);
 return 0;
#endif
}

static void extendDBRedis(lua_State* L)
{
    lua_pushstring(L, "DBRedis");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "command", tolua_liblua_DBRedis_command);
    }
    lua_pop(L, 1);
}

/* method: execute of class  DBMysql */
static int tolua_liblua_DBMysql_execute(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  size_t length;
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
  const char* query = ((const char*)  lua_tolstring(tolua_S,2,&length));
  int queryNumber = ((int)  tolua_tonumber(tolua_S,3,0));
  bool skipResult = ((bool)  tolua_toboolean(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'execute'", NULL);
#endif
  {
   return self->execute(tolua_S, query, length, queryNumber, skipResult);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'execute'.",&tolua_err);
 return 0;
#endif
}

/* method: escape of class  DBMysql */
static int tolua_liblua_DBMysql_escape(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  size_t length;
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
  const char* ptr = ((const char*)  lua_tolstring(tolua_S,2,&length));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'escape'", NULL);
#endif
  {
   return self->escape(tolua_S, ptr, length);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'escape'.",&tolua_err);
 return 0;
#endif
}

static void extendDBMysql(lua_State* L)
{
    lua_pushstring(L, "DBMysql");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "execute", tolua_liblua_DBMysql_execute);
        tolua_function(L, "escape", tolua_liblua_DBMysql_escape);
    }
    lua_pop(L, 1);
}


static int tolua_liblua_manual_open(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L,-1))//stack:...,_G,
    {
        extendHandler(L);
        extendDBRedis(L);
        extendDBMysql(L);
    }
    lua_pop(L, 1);
    return 1;
}

