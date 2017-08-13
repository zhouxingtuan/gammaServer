//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/29
// Time: 上午9:08
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__script__
#define __hive__script__

#include "core.h"
#include "lua.hpp"
#include "tolua++.h"
#include "luax.hpp"
#include "liblua.h"
extern "C" {
#include "mime.h"
}


NS_HIVE_BEGIN

class Script : public RefObject
{
public:
    Script(void);
    virtual ~Script(void);

	virtual void setState(lua_State* pState);
	inline lua_State * getState( void ) { return m_pState; }
	inline bool requireFile(const std::string& file){
		std::string exec = "require (\"" + file + "\")";
		return executeText(exec.c_str(), exec.length(), NULL);
	}
	inline bool executeFile( const char * script_file_path ){
		if( luaL_loadfile(m_pState, script_file_path ) == 0 ){
			if( lua_resume( m_pState, 0 ) == 0 ){
				return true;
			}
		}
		outputError();
		return false;
	}
	inline bool executeText( const char * script, const int script_length, const char * comment = NULL ){
		if( luaL_loadbuffer( m_pState, script, script_length, comment ) == 0 ){
			if( lua_pcall( m_pState, 0, 0, 0 ) == 0 ){
				return true;
			}
		}
		outputError();
		return false;
	}
    inline void luaCall( int parameter_number, int result_count = 0 ){
        if( lua_pcall( m_pState, parameter_number, result_count, 0 ) != 0 ){
            outputError();
        }
    }
	// 定制C++调用Lua的全局函数
	inline void clearStack(void){
		lua_settop(m_pState, 0);
	}
    inline void callFunction(const char * function_name, int result_count=0){
        lua_getglobal(m_pState, function_name);
        luaCall(0, result_count);
    }
	inline void callFunctionS(const char * function_name, const char* str, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushstring(m_pState, str);
		luaCall(1, result_count);
	}
	inline void callFunctionN(const char * function_name, uint32 number1, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		luaCall(1, result_count);
	}
	inline void callFunctionNN(const char * function_name, uint32 number1, uint32 number2, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		tolua_pushnumber(m_pState,(lua_Number)number2);
		luaCall(2, result_count);
	}
	inline void callFunctionNSN(const char * function_name, uint32 number1, const char* str, uint32 number2, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		tolua_pushstring(m_pState,str);
		tolua_pushnumber(m_pState,(lua_Number)number2);
		luaCall(3, result_count);
	}
	inline void callFunctionNNSN(const char * function_name, uint32 number1, uint32 number2, const char* str, uint32 number3, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		tolua_pushnumber(m_pState,(lua_Number)number2);
		tolua_pushstring(m_pState,str);
		tolua_pushnumber(m_pState,(lua_Number)number3);
		luaCall(4, result_count);
	}
	inline void callFunctionNR(const char * function_name, uint32 number1, const char* ptr, uint32 length, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		lua_pushlstring(m_pState,ptr,length);
		luaCall(2, result_count);
	}
	inline void callFunctionNNR(const char * function_name, uint32 number1, uint32 number2, const char* ptr, uint32 length, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		tolua_pushnumber(m_pState,(lua_Number)number2);
		lua_pushlstring(m_pState, ptr,length);
		luaCall(3, result_count);
	}
	inline void callFunctionBNR(const char * function_name, bool result, uint32 number1, const char* ptr, uint32 length, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushboolean(m_pState,result);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		lua_pushlstring(m_pState,ptr,length);
		luaCall(3, result_count);
	}
	inline void callFunctionUN(const char * function_name, void* ptr, const char* class_name, uint32 number1, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushusertype(m_pState, (void*)ptr, class_name);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		luaCall(2, result_count);
	}
	inline void callFunctionUR(const char * function_name, void* ptr, const char* class_name,
		const char* pData, uint32 length, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushusertype(m_pState, (void*)ptr, class_name);
		lua_pushlstring(m_pState,pData,length);
		luaCall(2, result_count);
	}
	inline void callFunctionUNR(const char * function_name, void* ptr, const char* class_name,
		uint32 number1, const char* pData, uint32 length, int result_count=0){
		lua_getglobal(m_pState, function_name);
		tolua_pushusertype(m_pState, (void*)ptr, class_name);
		tolua_pushnumber(m_pState,(lua_Number)number1);
		lua_pushlstring(m_pState,pData,length);
		luaCall(3, result_count);
	}

	// 从Lua函数返回结果中拿出数据
	inline double getDouble(int index, double value=0){
		return ((double)tolua_tonumber(m_pState, index, value));
	}
	inline int getInt64(int index, int value=0){
		return ((int64)tolua_tonumber(m_pState, index, value));
	}
	inline uint32 getUInt64(int index, int value=0){
		return ((uint64)tolua_tonumber(m_pState, index, value));
	}
	inline int getInt(int index, int value=0){
		return ((int)tolua_tonumber(m_pState, index, value));
	}
	inline uint32 getUInt(int index, int value=0){
		return ((uint32)tolua_tonumber(m_pState, index, value));
	}
	inline const char* getString(int index, uint32* length){
		return ((const char*)lua_tolstring(m_pState, index, (size_t*)length));
	}
	inline bool getBool(int index, bool value=false){
		return ((bool)tolua_toboolean(m_pState, index, value));
	}
	inline void outputError(void){
		const char* msg = lua_tostring( m_pState, -1 );
		if( msg == NULL )
			msg = "(error without message)";
		LOG_ERROR("lua error:%s\n", msg);
	}
protected:
	lua_State* m_pState;
};//end class Script

NS_HIVE_END

#endif
