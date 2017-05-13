//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/10
// Time: 上午8:30
// To change this template use File | Settings | File Templates.
//

#include "cppmodule.h"
#include "cpphandler.h"

NS_HIVE_BEGIN

void SOInitialize(void){
	LOG_DEBUG("cpp SOInitialize called");
}
void SODestroy(void){
	LOG_DEBUG("cpp SODestroy called");
}

Handler* HandlerCreateObject(uint32 index, uint32 poolType){
	LOG_DEBUG("cpp HandlerCreateObject called index=%d poolType=%d", index, poolType);
	CppHandler* pHandler = new CppHandler();
	pHandler->retain();
	return pHandler;
}

void HandlerReleaseObject(Handler* pObj){
	if(NULL != pObj){
		uint32 index = pObj->getIndex();
        uint32 poolType = pObj->getType();
		LOG_DEBUG("cpp HandlerReleaseObject called index=%d poolType=%d", index, poolType);
		pObj->release();
	}
}

NS_HIVE_END
