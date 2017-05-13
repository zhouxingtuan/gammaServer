//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/8
// Time: 上午11:54
// To change this template use File | Settings | File Templates.
//

#include "handlercreator.h"
#include <dlfcn.h>

NS_HIVE_BEGIN

Handler* HandlerCreatorCreateObject(uint32 index){
	switch(index){
		case MAIN_HANDLER_INDEX:{
			MainHandler* pMain = new MainHandler();
			pMain->retain();
			return pMain;
		}
		default:{
			LOG_INFO("create default handler in creator index=%d", index);
			// create default object
			MainHandler* pMain = new MainHandler();
			pMain->retain();
			return pMain;
		}
	}
	// should never come here
	MainHandler* pMain = new MainHandler();
	pMain->retain();
	return pMain;
}

void HandlerCreatorReleaseObject(Handler* pObj){
	if(NULL != pObj){
		pObj->release();
	}
}

HandlerCreator::HandlerCreator(void) : RefObject(), Sync(){

}
HandlerCreator::~HandlerCreator(void){

}

static HandlerCreator* g_pHandlerCreator = NULL;
HandlerCreator* HandlerCreator::getInstance(void){
	return g_pHandlerCreator;
}
HandlerCreator* HandlerCreator::createInstance(void){
	if(g_pHandlerCreator == NULL){
		g_pHandlerCreator = new HandlerCreator();
		SAFE_RETAIN(g_pHandlerCreator)
	}
	return g_pHandlerCreator;
}
void HandlerCreator::destroyInstance(void){
    SAFE_RELEASE(g_pHandlerCreator)
}

NS_HIVE_END
