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

Handler* HandlerCreatorCreateObject(uint32 index, uint32 poolType){
	if(poolType == HANDLER_TYPE_MAIN && index == MAIN_HANDLER_INDEX){
		MainHandler* pMain = new MainHandler();
		pMain->retain();
		return pMain;
	}else{
		return HandlerCreator::getInstance()->createObject(index, poolType);
	}
}

void HandlerCreatorReleaseObject(Handler* pObj){
	if(NULL == pObj){
		return;
	}
	uint32 index = pObj->getIndex();
	uint32 poolType = pObj->getType();
	if(poolType == HANDLER_TYPE_MAIN && index == MAIN_HANDLER_INDEX){
		pObj->release();
	}else{
		HandlerCreator::getInstance()->releaseObject(pObj);
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

void parseBeginEnd(const std::string& beginEndStr, uint32& beginIndex, uint32& endIndex){
	Token::TokenMap begin_end_map;
	Token::split(beginEndStr, "-", begin_end_map);
	for(auto &kv : begin_end_map){
		beginIndex = atoi(kv.first.c_str());
		endIndex = atoi(kv.second.c_str());
		LOG_DEBUG("beginIndex=%d endIndex=%d", beginIndex, endIndex);
		return;
	}
}
void HandlerCreator::initializeSO(Token::TokenMap& config){
	char key[64];
	int module_count = atoi(config["module_count"].c_str());
	LOG_DEBUG("initializeSO module_count=%d", module_count);
	for(int i=1; i<=module_count; ++i){
		sprintf(key, "module_name_%d", i);
		std::string name = config[key];
		sprintf(key, "command_%d", i);
		uint32 command = atoi(config[key].c_str());
		if(command >= COMMAND_NUMBER){
			LOG_ERROR("name=%s command >= COMMAND_NUMBER(%d)", name.c_str(), COMMAND_NUMBER);
			continue;
		}
		sprintf(key, "pool_type_%d", i);
		uint32 poolType = atoi(config[key].c_str());
		if(poolType >= DESTINATION_MAX_GROUP){
			LOG_ERROR("name=%s poolType >= DESTINATION_MAX_GROUP(%d)", name.c_str(), DESTINATION_MAX_GROUP);
			continue;
		}
		sprintf(key, "handler_index_%d", i);
		std::string beginEndStr = config[key];
		uint32 beginIndex = 0;
		uint32 endIndex = 0;
		parseBeginEnd(beginEndStr, beginIndex, endIndex);
		loadSO(name, command, poolType, beginIndex, endIndex);
	}
}
void HandlerCreator::loadSO(const std::string& name, uint32 command, uint32 poolType, uint32 beginIndex, uint32 endIndex){
	LOG_DEBUG("load SO name=%s command=%d poolType=%d beginIndex=%d endIndex=%d", name.c_str(), command, poolType, beginIndex, endIndex);
	SOInformation* pInfo = new SOInformation();
	pInfo->command = command;
	pInfo->poolType = poolType;
	pInfo->beginIndex = beginIndex;
	pInfo->endIndex = endIndex;
	void* pHandle;
	lock();
	unloadSO(name);
	do{
		pHandle = dlopen(name.c_str(), RTLD_NOW);
		if(NULL == pHandle){
			LOG_ERROR("load dll failed");
			break;
		}
		pInfo->pHandle = pHandle;
		pInfo->createFunc = (SOCreateObjectFunction)dlsym(pHandle, "HandlerCreateObject");
		if(dlerror() != NULL){
		  LOG_ERROR("load dll func failed HandlerCreateObject");
		  break;
		}
		pInfo->releaseFunc = (SOReleaseObjectFunction)dlsym(pHandle, "HandlerReleaseObject");
		if(dlerror() != NULL){
		  LOG_ERROR("load dll func failed HandlerReleaseObject");
		  break;
		}
		pInfo->initializeFunc = (SOInitializeFunction)dlsym(pHandle, "SOInitialize");
		if(dlerror() != NULL){
		  LOG_ERROR("load dll func failed SOInitialize");
		  break;
		}
		pInfo->destroyFunc = (SODestroyFunction)dlsym(pHandle, "SODestroy");
		if(dlerror() != NULL){
		  LOG_ERROR("load dll func failed SODestroy");
		  break;
		}
		m_soInfoMap.insert(std::make_pair(name, pInfo));
		LOG_INFO("load SO OK name=%s", name.c_str());
		pInfo->initializeFunc();
		for(uint32 i=beginIndex; i<=endIndex; ++i){
            GlobalHandler::getInstance()->createDestination(poolType, i);
        }
        unlock();
		return;
	}while(0);
	if(pInfo->pHandle != NULL){
		dlclose(pInfo->pHandle);
	}
	delete pInfo;
	unlock();
	LOG_ERROR("load SO failed name=%s", name.c_str());
}
void HandlerCreator::unloadSO(const std::string& name){
	LOG_DEBUG("unload SO name=%s", name.c_str());
	SOInformationMap::iterator itCur;
	SOInformation* pInfo = NULL;
	itCur = m_soInfoMap.find(name);
	if(itCur != m_soInfoMap.end()){
		pInfo = itCur->second;
		// unload all the objects
		unloadSOInformation(pInfo);
		// erase from map
		m_soInfoMap.erase(itCur);
	}
}
void HandlerCreator::unloadAll(void){
	for(auto &kv : m_soInfoMap){
		SOInformation* pInfo = kv.second;
		unloadSOInformation(pInfo);
	}
	m_soInfoMap.clear();
}
void HandlerCreator::unloadSOInformation(SOInformation* pInfo){
	uint32 beginIndex = pInfo->beginIndex;
	uint32 endIndex = pInfo->endIndex;
	uint32 poolType = pInfo->poolType;
	for(uint32 i=beginIndex; i<=endIndex; ++i){
		GlobalHandler::getInstance()->removeDestinationByIndex(poolType, i);
	}
	pInfo->destroyFunc();
	// close the SO file
	dlclose(pInfo->pHandle);
	delete pInfo;
}
Handler* HandlerCreator::createObject(uint32 index, uint32 poolType){
	SOInformation* pInfo = findMatchInfo(index, poolType);
	if(NULL != pInfo){
		Handler* pHandler = pInfo->createFunc(index, poolType);
		if(NULL != pHandler && pInfo->command > COMMAND_HIVE_RESPONSE){
			// register Handler to the command Dispatcher
			uint32 handle = pHandler->getHandle();
			Dispatcher::getInstance()->appendCommandListener(pInfo->command, handle);
		}
		return pHandler;
	}
	return NULL;
}
void HandlerCreator::releaseObject(Handler* pObj){
	uint32 index = pObj->getIndex();
	uint32 poolType = pObj->getType();
	SOInformation* pInfo = findMatchInfo(index, poolType);
	if(NULL != pInfo){
		// unregister Handler from the command Dispatcher
		uint32 handle = pObj->getHandle();
		Dispatcher::getInstance()->removeCommandListener(pInfo->command, handle);
		pInfo->releaseFunc(pObj);
	}
}
SOInformation* HandlerCreator::findMatchInfo(uint32 index, uint32 poolType){
	for(auto &kv : m_soInfoMap){
		SOInformation* pInfo = kv.second;
		if(pInfo->poolType == poolType && index >= pInfo->beginIndex && index <= pInfo->endIndex){
			return pInfo;
		}
	}
	return NULL;
}










NS_HIVE_END
