//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/29
// Time: 上午9:10
// To change this template use File | Settings | File Templates.
//

#include "eproto.h"
#include "manual.hpp"
#include "script.h"

NS_HIVE_BEGIN

Script::Script(void) : RefObject(), m_pState(NULL) {

}
Script::~Script(void){
	if(NULL != m_pState){
		lua_close(m_pState); // 先关闭自己
	}
}
void Script::setState(lua_State* pState){
	if(pState==NULL){
		m_pState = luaL_newstate();
		lua_pop( m_pState, luaopen_base( m_pState ) );
		lua_pop( m_pState, luaopen_math( m_pState ) );
		lua_pop( m_pState, 1 );
		// Save a pointer to the thread Manager object in the global table
		// using the new thread script vm pointer as a key.
		lua_pushlightuserdata( m_pState, m_pState );
		lua_pushlightuserdata( m_pState, this );
		lua_settable( m_pState, LUA_GLOBALSINDEX );
	}else{
		m_pState = pState;
	}
	// open standard libs
	luaL_openlibs( m_pState );
	lua_settop( m_pState, 0 );
	// open mime
	luaopen_mime_core(m_pState);
	// open rapidjson
	luaopen_rapidjson(m_pState);
	// open eproto
	luaopen_eproto_cpp(m_pState);
	// open my lua lib
	tolua_liblua_open(m_pState);
	// extend libs
	tolua_liblua_manual_open(m_pState);
}

NS_HIVE_END
