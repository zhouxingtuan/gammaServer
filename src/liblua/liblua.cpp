/*
** Lua binding: liblua
** Generated automatically by tolua++-1.0.92 on Sun May 14 11:54:26 2017.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_liblua_open (lua_State* tolua_S);

#include "core.h"
#include "manual.hpp"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"Handler");
}

/* method: getHandle of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getHandle00
static int tolua_liblua_Handler_getHandle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Handler",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Handler* self = (const Handler*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getHandle'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getHandle();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getHandle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: identifyHive of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_identifyHive00
static int tolua_liblua_Handler_identifyHive00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'identifyHive'", NULL);
#endif
  {
   self->identifyHive(connectHandle);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'identifyHive'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: openClient of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_openClient00
static int tolua_liblua_Handler_openClient00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,5,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  const char* ip = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned short port = (( unsigned short)  tolua_tonumber(tolua_S,4,0));
  bool isNeedEncrypt = ((bool)  tolua_toboolean(tolua_S,5,0));
  bool isNeedDecrypt = ((bool)  tolua_toboolean(tolua_S,6,0));
  unsigned char acceptIndex = (( unsigned char)  tolua_tonumber(tolua_S,7,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'openClient'", NULL);
#endif
  {
   self->openClient(callbackID,ip,port,isNeedEncrypt,isNeedDecrypt,acceptIndex);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'openClient'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: openHttpListener of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_openHttpListener00
static int tolua_liblua_Handler_openHttpListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  const char* ip = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned short port = (( unsigned short)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'openHttpListener'", NULL);
#endif
  {
   self->openHttpListener(callbackID,ip,port);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'openHttpListener'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: openHttpsListener of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_openHttpsListener00
static int tolua_liblua_Handler_openHttpsListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  const char* ip = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned short port = (( unsigned short)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'openHttpsListener'", NULL);
#endif
  {
   self->openHttpsListener(callbackID,ip,port);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'openHttpsListener'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: openSocketListener of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_openSocketListener00
static int tolua_liblua_Handler_openSocketListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,5,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  const char* ip = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned short port = (( unsigned short)  tolua_tonumber(tolua_S,4,0));
  bool isNeedEncrypt = ((bool)  tolua_toboolean(tolua_S,5,0));
  bool isNeedDecrypt = ((bool)  tolua_toboolean(tolua_S,6,0));
  unsigned char acceptIndex = (( unsigned char)  tolua_tonumber(tolua_S,7,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'openSocketListener'", NULL);
#endif
  {
   self->openSocketListener(callbackID,ip,port,isNeedEncrypt,isNeedDecrypt,acceptIndex);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'openSocketListener'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: closeListener of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_closeListener00
static int tolua_liblua_Handler_closeListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int listenerHandle = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'closeListener'", NULL);
#endif
  {
   self->closeListener(callbackID,listenerHandle);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'closeListener'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: closeConnect of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_closeConnect00
static int tolua_liblua_Handler_closeConnect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'closeConnect'", NULL);
#endif
  {
   self->closeConnect(callbackID,connectHandle);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'closeConnect'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: bindAccept of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_bindAccept00
static int tolua_liblua_Handler_bindAccept00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int acceptHandle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int bindHandle = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'bindAccept'", NULL);
#endif
  {
   self->bindAccept(acceptHandle,bindHandle);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'bindAccept'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: startTimer of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_startTimer00
static int tolua_liblua_Handler_startTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int callbackID = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  long long int timeCount = (( long long int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'startTimer'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->startTimer(callbackID,timeCount);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'startTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeTimer of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_removeTimer00
static int tolua_liblua_Handler_removeTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int handle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeTimer'", NULL);
#endif
  {
   self->removeTimer(handle);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: changeTimer of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_changeTimer00
static int tolua_liblua_Handler_changeTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int handle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  long long int timeCount = (( long long int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'changeTimer'", NULL);
#endif
  {
   self->changeTimer(handle,timeCount);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'changeTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: leftTimer of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_leftTimer00
static int tolua_liblua_Handler_leftTimer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int handle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'leftTimer'", NULL);
#endif
  {
   long long int tolua_ret = ( long long int)  self->leftTimer(handle);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'leftTimer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_liblua_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"Handler","Handler","",NULL);
  tolua_beginmodule(tolua_S,"Handler");
   tolua_function(tolua_S,"getHandle",tolua_liblua_Handler_getHandle00);
   tolua_function(tolua_S,"identifyHive",tolua_liblua_Handler_identifyHive00);
   tolua_function(tolua_S,"openClient",tolua_liblua_Handler_openClient00);
   tolua_function(tolua_S,"openHttpListener",tolua_liblua_Handler_openHttpListener00);
   tolua_function(tolua_S,"openHttpsListener",tolua_liblua_Handler_openHttpsListener00);
   tolua_function(tolua_S,"openSocketListener",tolua_liblua_Handler_openSocketListener00);
   tolua_function(tolua_S,"closeListener",tolua_liblua_Handler_closeListener00);
   tolua_function(tolua_S,"closeConnect",tolua_liblua_Handler_closeConnect00);
   tolua_function(tolua_S,"bindAccept",tolua_liblua_Handler_bindAccept00);
   tolua_function(tolua_S,"startTimer",tolua_liblua_Handler_startTimer00);
   tolua_function(tolua_S,"removeTimer",tolua_liblua_Handler_removeTimer00);
   tolua_function(tolua_S,"changeTimer",tolua_liblua_Handler_changeTimer00);
   tolua_function(tolua_S,"leftTimer",tolua_liblua_Handler_leftTimer00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_liblua (lua_State* tolua_S) {
 return tolua_liblua_open(tolua_S);
};
#endif

