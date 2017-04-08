//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/8
// Time: 上午11:54
// To change this template use File | Settings | File Templates.
//

#include "handlercreator.h"

NS_HIVE_BEGIN

Destination* HandlerCreatorCreateObject(Destination::index_type index){
	switch(index){
		case MAIN_HANDLER_INDEX:{
			return new MainHandler();
		}
		default:{
			LOG_INFO("create default handler in creator index=%d", index);
			// create default object
			return new MainHandler();
		}
	}
	// should never come here
	return new MainHandler();
}

void HandlerCreatorReleaseObject(Destination* pObj){
	if(NULL != pObj){
		delete pObj;
	}
}

NS_HIVE_END
