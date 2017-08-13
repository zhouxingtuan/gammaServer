/*
** Lua binding: liblua
** Generated automatically by tolua++-1.0.92 on Sun Aug 13 22:24:45 2017.
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
 tolua_usertype(tolua_S,"DBMysql");
 tolua_usertype(tolua_S,"MD5");
 tolua_usertype(tolua_S,"DBRedis");
 tolua_usertype(tolua_S,"LuaHandler");
 tolua_usertype(tolua_S,"Handler");
}

/* method: getService of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getService00
static int tolua_liblua_Handler_getService00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getService'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->getService();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getService'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getType of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getType00
static int tolua_liblua_Handler_getType00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getType'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->getType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getIndex of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getIndex00
static int tolua_liblua_Handler_getIndex00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getIndex'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->getIndex();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getNode of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getNode00
static int tolua_liblua_Handler_getNode00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getNode'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->getNode();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getNode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

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
   unsigned int tolua_ret = (unsigned int)  self->getHandle();
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

/* method: getModuleType of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getModuleType00
static int tolua_liblua_Handler_getModuleType00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getModuleType'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getModuleType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getModuleType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getModuleIndex of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getModuleIndex00
static int tolua_liblua_Handler_getModuleIndex00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getModuleIndex'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getModuleIndex();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getModuleIndex'.",&tolua_err);
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

/* method: sendToNode of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_sendToNode00
static int tolua_liblua_Handler_sendToNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int command = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int destination = (( unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendToNode'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->sendToNode(command,connectHandle,destination,ptr,length);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendToNode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendToDestination of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_sendToDestination00
static int tolua_liblua_Handler_sendToDestination00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int destination = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,4,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendToDestination'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->sendToDestination(connectHandle,destination,ptr,length);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendToDestination'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: responseDestination of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_responseDestination00
static int tolua_liblua_Handler_responseDestination00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int destination = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,4,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'responseDestination'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->responseDestination(connectHandle,destination,ptr,length);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'responseDestination'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendToGroup of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_sendToGroup00
static int tolua_liblua_Handler_sendToGroup00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* groupName = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* moduleName = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned int moduleIndex = (( unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendToGroup'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->sendToGroup(groupName,moduleName,moduleIndex,ptr,length);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendToGroup'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: responseGroup of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_responseGroup00
static int tolua_liblua_Handler_responseGroup00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int destination = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,4,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'responseGroup'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->responseGroup(connectHandle,destination,ptr,length);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'responseGroup'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addGroup of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_addGroup00
static int tolua_liblua_Handler_addGroup00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* groupName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addGroup'", NULL);
#endif
  {
   self->addGroup(groupName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addGroup'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeGroup of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_removeGroup00
static int tolua_liblua_Handler_removeGroup00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* groupName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeGroup'", NULL);
#endif
  {
   self->removeGroup(groupName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeGroup'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getGroupRoute of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getGroupRoute00
static int tolua_liblua_Handler_getGroupRoute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* groupName = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* moduleName = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int command = (( unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getGroupRoute'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->getGroupRoute(groupName,moduleName,connectHandle,command);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushnumber(tolua_S,(lua_Number)connectHandle);
   tolua_pushnumber(tolua_S,(lua_Number)command);
  }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getGroupRoute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setGroupRoute of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_setGroupRoute00
static int tolua_liblua_Handler_setGroupRoute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* groupName = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setGroupRoute'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->setGroupRoute(groupName,connectHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setGroupRoute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeGroupRoute of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_removeGroupRoute00
static int tolua_liblua_Handler_removeGroupRoute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* groupName = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int connectHandle = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeGroupRoute'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->removeGroupRoute(groupName,connectHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeGroupRoute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: appendGroupModule of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_appendGroupModule00
static int tolua_liblua_Handler_appendGroupModule00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* groupName = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* moduleName = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned int command = (( unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'appendGroupModule'", NULL);
#endif
  {
   self->appendGroupModule(groupName,moduleName,command);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'appendGroupModule'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeGroupModule of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_removeGroupModule00
static int tolua_liblua_Handler_removeGroupModule00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* groupName = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* moduleName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeGroupModule'", NULL);
#endif
  {
   self->removeGroupModule(groupName,moduleName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeGroupModule'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getModuleSize of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getModuleSize00
static int tolua_liblua_Handler_getModuleSize00(lua_State* tolua_S)
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
  unsigned int moduleType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getModuleSize'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getModuleSize(moduleType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getModuleSize'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: randModuleHandle of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_randModuleHandle00
static int tolua_liblua_Handler_randModuleHandle00(lua_State* tolua_S)
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
  unsigned int moduleType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'randModuleHandle'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->randModuleHandle(moduleType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'randModuleHandle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getModuleHandle of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_getModuleHandle00
static int tolua_liblua_Handler_getModuleHandle00(lua_State* tolua_S)
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
  unsigned int moduleType = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int moduleIndex = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getModuleHandle'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->getModuleHandle(moduleType,moduleIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getModuleHandle'.",&tolua_err);
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

/* method: hash of class  Handler */
#ifndef TOLUA_DISABLE_tolua_liblua_Handler_hash00
static int tolua_liblua_Handler_hash00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Handler",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Handler* self = (Handler*)  tolua_tousertype(tolua_S,1,0);
  const char* str = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int length = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'hash'", NULL);
#endif
  {
   unsigned int tolua_ret = ( unsigned int)  self->hash(str,length);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'hash'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: update of class  MD5 */
#ifndef TOLUA_DISABLE_tolua_liblua_MD5_update00
static int tolua_liblua_MD5_update00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MD5",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MD5* self = (MD5*)  tolua_tousertype(tolua_S,1,0);
  const char* ptr = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int length = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'update'", NULL);
#endif
  {
   self->update(ptr,length);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'update'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getString of class  MD5 */
#ifndef TOLUA_DISABLE_tolua_liblua_MD5_getString00
static int tolua_liblua_MD5_getString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MD5",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MD5* self = (MD5*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->getString();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: connectMySql of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_connectMySql00
static int tolua_liblua_DBMysql_connectMySql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
  const char* ip = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* user = ((const char*)  tolua_tostring(tolua_S,3,0));
  const char* pwd = ((const char*)  tolua_tostring(tolua_S,4,0));
  const char* db = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned short port = (( unsigned short)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'connectMySql'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->connectMySql(ip,user,pwd,db,port);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'connectMySql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: closeMysql of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_closeMysql00
static int tolua_liblua_DBMysql_closeMysql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'closeMysql'", NULL);
#endif
  {
   self->closeMysql();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'closeMysql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pingMysql of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_pingMysql00
static int tolua_liblua_DBMysql_pingMysql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pingMysql'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->pingMysql();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pingMysql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getLastAutoID of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_getLastAutoID00
static int tolua_liblua_DBMysql_getLastAutoID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getLastAutoID'", NULL);
#endif
  {
   long long int tolua_ret = ( long long int)  self->getLastAutoID();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getLastAutoID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setAutoCommit of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_setAutoCommit00
static int tolua_liblua_DBMysql_setAutoCommit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
  bool on = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setAutoCommit'", NULL);
#endif
  {
   self->setAutoCommit(on);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setAutoCommit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: commit of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_commit00
static int tolua_liblua_DBMysql_commit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'commit'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->commit();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'commit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: rollback of class  DBMysql */
#ifndef TOLUA_DISABLE_tolua_liblua_DBMysql_rollback00
static int tolua_liblua_DBMysql_rollback00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBMysql* self = (DBMysql*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'rollback'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->rollback();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'rollback'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: connectRedis of class  DBRedis */
#ifndef TOLUA_DISABLE_tolua_liblua_DBRedis_connectRedis00
static int tolua_liblua_DBRedis_connectRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBRedis",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBRedis* self = (DBRedis*)  tolua_tousertype(tolua_S,1,0);
  const char* ip = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned short port = (( unsigned short)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'connectRedis'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->connectRedis(ip,port);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'connectRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: closeRedis of class  DBRedis */
#ifndef TOLUA_DISABLE_tolua_liblua_DBRedis_closeRedis00
static int tolua_liblua_DBRedis_closeRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBRedis",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBRedis* self = (DBRedis*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'closeRedis'", NULL);
#endif
  {
   self->closeRedis();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'closeRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: identifyRedis of class  DBRedis */
#ifndef TOLUA_DISABLE_tolua_liblua_DBRedis_identifyRedis00
static int tolua_liblua_DBRedis_identifyRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DBRedis",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DBRedis* self = (DBRedis*)  tolua_tousertype(tolua_S,1,0);
  const char* pwd = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'identifyRedis'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->identifyRedis(pwd);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'identifyRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: getTimeStringUS */
#ifndef TOLUA_DISABLE_tolua_liblua_getTimeStringUS00
static int tolua_liblua_getTimeStringUS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   const char* tolua_ret = (const char*)  getTimeStringUS();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTimeStringUS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createRedis of class  LuaHandler */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaHandler_createRedis00
static int tolua_liblua_LuaHandler_createRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaHandler",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaHandler* self = (LuaHandler*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createRedis'", NULL);
#endif
  {
   DBRedis* tolua_ret = (DBRedis*)  self->createRedis();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"DBRedis");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeRedis of class  LuaHandler */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaHandler_removeRedis00
static int tolua_liblua_LuaHandler_removeRedis00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaHandler",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"DBRedis",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaHandler* self = (LuaHandler*)  tolua_tousertype(tolua_S,1,0);
  DBRedis* pDBRedis = ((DBRedis*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeRedis'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->removeRedis(pDBRedis);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeRedis'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createMysql of class  LuaHandler */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaHandler_createMysql00
static int tolua_liblua_LuaHandler_createMysql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaHandler",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaHandler* self = (LuaHandler*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createMysql'", NULL);
#endif
  {
   DBMysql* tolua_ret = (DBMysql*)  self->createMysql();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"DBMysql");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createMysql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeMysql of class  LuaHandler */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaHandler_removeMysql00
static int tolua_liblua_LuaHandler_removeMysql00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaHandler",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"DBMysql",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaHandler* self = (LuaHandler*)  tolua_tousertype(tolua_S,1,0);
  DBMysql* pDBMysql = ((DBMysql*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeMysql'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->removeMysql(pDBMysql);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeMysql'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createMD5 of class  LuaHandler */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaHandler_createMD500
static int tolua_liblua_LuaHandler_createMD500(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaHandler",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaHandler* self = (LuaHandler*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createMD5'", NULL);
#endif
  {
   MD5* tolua_ret = (MD5*)  self->createMD5();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"MD5");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createMD5'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeMD5 of class  LuaHandler */
#ifndef TOLUA_DISABLE_tolua_liblua_LuaHandler_removeMD500
static int tolua_liblua_LuaHandler_removeMD500(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaHandler",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"MD5",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaHandler* self = (LuaHandler*)  tolua_tousertype(tolua_S,1,0);
  MD5* pMD5 = ((MD5*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'removeMD5'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->removeMD5(pMD5);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'removeMD5'.",&tolua_err);
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
   tolua_function(tolua_S,"getService",tolua_liblua_Handler_getService00);
   tolua_function(tolua_S,"getType",tolua_liblua_Handler_getType00);
   tolua_function(tolua_S,"getIndex",tolua_liblua_Handler_getIndex00);
   tolua_function(tolua_S,"getNode",tolua_liblua_Handler_getNode00);
   tolua_function(tolua_S,"getHandle",tolua_liblua_Handler_getHandle00);
   tolua_function(tolua_S,"getModuleType",tolua_liblua_Handler_getModuleType00);
   tolua_function(tolua_S,"getModuleIndex",tolua_liblua_Handler_getModuleIndex00);
   tolua_function(tolua_S,"identifyHive",tolua_liblua_Handler_identifyHive00);
   tolua_function(tolua_S,"openClient",tolua_liblua_Handler_openClient00);
   tolua_function(tolua_S,"openHttpListener",tolua_liblua_Handler_openHttpListener00);
   tolua_function(tolua_S,"openHttpsListener",tolua_liblua_Handler_openHttpsListener00);
   tolua_function(tolua_S,"openSocketListener",tolua_liblua_Handler_openSocketListener00);
   tolua_function(tolua_S,"closeListener",tolua_liblua_Handler_closeListener00);
   tolua_function(tolua_S,"closeConnect",tolua_liblua_Handler_closeConnect00);
   tolua_function(tolua_S,"bindAccept",tolua_liblua_Handler_bindAccept00);
   tolua_function(tolua_S,"sendToNode",tolua_liblua_Handler_sendToNode00);
   tolua_function(tolua_S,"sendToDestination",tolua_liblua_Handler_sendToDestination00);
   tolua_function(tolua_S,"responseDestination",tolua_liblua_Handler_responseDestination00);
   tolua_function(tolua_S,"sendToGroup",tolua_liblua_Handler_sendToGroup00);
   tolua_function(tolua_S,"responseGroup",tolua_liblua_Handler_responseGroup00);
   tolua_function(tolua_S,"addGroup",tolua_liblua_Handler_addGroup00);
   tolua_function(tolua_S,"removeGroup",tolua_liblua_Handler_removeGroup00);
   tolua_function(tolua_S,"getGroupRoute",tolua_liblua_Handler_getGroupRoute00);
   tolua_function(tolua_S,"setGroupRoute",tolua_liblua_Handler_setGroupRoute00);
   tolua_function(tolua_S,"removeGroupRoute",tolua_liblua_Handler_removeGroupRoute00);
   tolua_function(tolua_S,"appendGroupModule",tolua_liblua_Handler_appendGroupModule00);
   tolua_function(tolua_S,"removeGroupModule",tolua_liblua_Handler_removeGroupModule00);
   tolua_function(tolua_S,"getModuleSize",tolua_liblua_Handler_getModuleSize00);
   tolua_function(tolua_S,"randModuleHandle",tolua_liblua_Handler_randModuleHandle00);
   tolua_function(tolua_S,"getModuleHandle",tolua_liblua_Handler_getModuleHandle00);
   tolua_function(tolua_S,"startTimer",tolua_liblua_Handler_startTimer00);
   tolua_function(tolua_S,"removeTimer",tolua_liblua_Handler_removeTimer00);
   tolua_function(tolua_S,"changeTimer",tolua_liblua_Handler_changeTimer00);
   tolua_function(tolua_S,"leftTimer",tolua_liblua_Handler_leftTimer00);
   tolua_function(tolua_S,"hash",tolua_liblua_Handler_hash00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"MD5","MD5","",NULL);
  tolua_beginmodule(tolua_S,"MD5");
   tolua_function(tolua_S,"update",tolua_liblua_MD5_update00);
   tolua_function(tolua_S,"getString",tolua_liblua_MD5_getString00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DBMysql","DBMysql","",NULL);
  tolua_beginmodule(tolua_S,"DBMysql");
   tolua_function(tolua_S,"connectMySql",tolua_liblua_DBMysql_connectMySql00);
   tolua_function(tolua_S,"closeMysql",tolua_liblua_DBMysql_closeMysql00);
   tolua_function(tolua_S,"pingMysql",tolua_liblua_DBMysql_pingMysql00);
   tolua_function(tolua_S,"getLastAutoID",tolua_liblua_DBMysql_getLastAutoID00);
   tolua_function(tolua_S,"setAutoCommit",tolua_liblua_DBMysql_setAutoCommit00);
   tolua_function(tolua_S,"commit",tolua_liblua_DBMysql_commit00);
   tolua_function(tolua_S,"rollback",tolua_liblua_DBMysql_rollback00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DBRedis","DBRedis","",NULL);
  tolua_beginmodule(tolua_S,"DBRedis");
   tolua_function(tolua_S,"connectRedis",tolua_liblua_DBRedis_connectRedis00);
   tolua_function(tolua_S,"closeRedis",tolua_liblua_DBRedis_closeRedis00);
   tolua_function(tolua_S,"identifyRedis",tolua_liblua_DBRedis_identifyRedis00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"getTimeStringUS",tolua_liblua_getTimeStringUS00);
  tolua_cclass(tolua_S,"LuaHandler","LuaHandler","Handler",NULL);
  tolua_beginmodule(tolua_S,"LuaHandler");
   tolua_function(tolua_S,"createRedis",tolua_liblua_LuaHandler_createRedis00);
   tolua_function(tolua_S,"removeRedis",tolua_liblua_LuaHandler_removeRedis00);
   tolua_function(tolua_S,"createMysql",tolua_liblua_LuaHandler_createMysql00);
   tolua_function(tolua_S,"removeMysql",tolua_liblua_LuaHandler_removeMysql00);
   tolua_function(tolua_S,"createMD5",tolua_liblua_LuaHandler_createMD500);
   tolua_function(tolua_S,"removeMD5",tolua_liblua_LuaHandler_removeMD500);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_liblua (lua_State* tolua_S) {
 return tolua_liblua_open(tolua_S);
};
#endif

