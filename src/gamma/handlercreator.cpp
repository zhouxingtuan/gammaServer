//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/8
// Time: 上午11:54
// To change this template use File | Settings | File Templates.
//

#include "handlercreator.h"

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

NS_HIVE_END
