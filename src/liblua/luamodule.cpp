//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/10
// Time: 上午8:30
// To change this template use File | Settings | File Templates.
//

#include "luamodule.h"
#include "luahandler.h"

NS_HIVE_BEGIN

void SOInitialize(void){
	LOG_DEBUG("lua SOInitialize called");
}
void SODestroy(void){
	LOG_DEBUG("lua SODestroy called");
}

Handler* HandlerCreateObject(uint32 index, uint32 poolType){
	LOG_DEBUG("lua HandlerCreateObject called index=%d poolType=%d", index, poolType);
	LuaHandler* pHandler = new LuaHandler();
	pHandler->retain();
	return pHandler;
}

void HandlerReleaseObject(Handler* pObj){
	if(NULL != pObj){
		uint32 index = pObj->getIndex();
        uint32 poolType = pObj->getType();
		LOG_DEBUG("lua HandlerReleaseObject called index=%d poolType=%d", index, poolType);
		pObj->release();
	}
}

NS_HIVE_END
