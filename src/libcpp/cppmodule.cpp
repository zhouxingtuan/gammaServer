//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/10
// Time: 上午8:30
// To change this template use File | Settings | File Templates.
//

#include "cppmodule.h"

NS_HIVE_BEGIN

void SOInitialize(void){

}
void SODestroy(void){

}

Handler* HandlerCreateObject(uint32 index){

	return NULL;
}

void HandlerReleaseObject(Handler* pObj){
	if(NULL != pObj){
		pObj->release();
	}
}

NS_HIVE_END
