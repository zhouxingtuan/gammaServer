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
		sprintf(key, "module_so_%d", i);
		std::string soName = config[key];
		sprintf(key, "module_param_%d", i);
		std::string param = config[key];
		sprintf(key, "module_type_%d", i);
		uint32 moduleType = atoi(config[key].c_str());
		if(moduleType >= COMMAND_NUMBER){
			LOG_ERROR("moduleType=%d moduleType >= COMMAND_NUMBER(%d)", moduleType, COMMAND_NUMBER);
			continue;
		}
		sprintf(key, "module_version_%d", i);
		uint32 moduleVersion = atoi(config[key].c_str());
		sprintf(key, "module_index_%d", i);
		std::string beginEndStr = config[key];
		uint32 beginModuleIndex = 0;
		uint32 endModuleIndex = 0;
		parseBeginEnd(beginEndStr, beginModuleIndex, endModuleIndex);
		loadModule(soName, param, moduleVersion, moduleType, beginModuleIndex, endModuleIndex);
	}
}
void HandlerCreator::loadModule(const std::string& soName, const std::string& param, uint32 moduleVersion,
		uint32 moduleType, uint32 beginModuleIndex, uint32 endModuleIndex){
	LOG_DEBUG("load SO soName=%s moduleType=%d beginModuleIndex=%d endModuleIndex=%d",
		soName.c_str(), moduleType, beginModuleIndex, endModuleIndex);
	unloadModule(moduleType);
	loadSO(soName);
	// 预先分配一个区间的index
	uint32 number = endModuleIndex + 1 - beginModuleIndex;
	uint32 endIndex = GlobalHandler::getInstance()->getNextDestinationIndex(HANDLER_TYPE_MAIN, number);
	uint32 beginIndex = endIndex + 1 - number;
	if( beginIndex > endIndex ){
		LOG_ERROR("loadModule failed beginIndex=%d < endIndex=%d", beginIndex, endIndex);
		return;
	}

	ModuleInformation* pModuleInfo = new ModuleInformation();
	pModuleInfo->soName = soName;
	pModuleInfo->moduleType = moduleType;
	pModuleInfo->poolType = HANDLER_TYPE_MAIN;
	pModuleInfo->beginIndex = beginIndex;
	pModuleInfo->endIndex = endIndex;
	m_moduleInfoMap.insert(std::make_pair(moduleType, pModuleInfo));
	// load the modules
	uint32 index = beginIndex;
	for(uint32 moduleIndex=beginModuleIndex; moduleIndex<=endModuleIndex; ++moduleIndex){
        uint32 handle = GlobalHandler::getInstance()->createDestination(HANDLER_TYPE_MAIN, index, moduleType, moduleIndex, param);
        ++index;
        // 添加handler到分组管理
		GlobalModule::getInstance()->addInformation(moduleType, moduleIndex, handle, moduleVersion);
	}
}
void HandlerCreator::unloadModule(uint32 moduleType){
	ModuleInformationMap::iterator itCur = m_moduleInfoMap.find(moduleType);
	if(itCur != m_moduleInfoMap.end()){
		unloadModuleInformation(itCur->second);
		m_moduleInfoMap.erase(itCur);
	}
}
void HandlerCreator::unloadModuleInformation(ModuleInformation* pModuleInfo){
	uint32 beginIndex = pModuleInfo->beginIndex;
    uint32 endIndex = pModuleInfo->endIndex;
    uint32 poolType = pModuleInfo->poolType;
    for(uint32 i=beginIndex; i<=endIndex; ++i){
        GlobalHandler::getInstance()->removeDestinationByIndex(poolType, i);
    }
    delete pModuleInfo;
}
void HandlerCreator::loadSO(const std::string& name){
	LOG_DEBUG("load SO name=%s", name.c_str());
	SOInformationMap::iterator itCur = m_soInfoMap.find(name);
	if(itCur != m_soInfoMap.end())
	{
		return;
	}
	SOInformation* pInfo = new SOInformation();
	pInfo->soName = name;
	void* pHandle;
	lock();
	do{
		char* errorStr;
		pHandle = dlopen(name.c_str(), RTLD_NOW);
		if(NULL == pHandle){
			LOG_ERROR("load dll failed error=%s", dlerror());
			break;
		}
		pInfo->pHandle = pHandle;
		pInfo->createFunc = (SOCreateObjectFunction)dlsym(pHandle, "HandlerCreateObject");
		errorStr = dlerror();
		if(errorStr != NULL){
		  LOG_ERROR("load dll func failed HandlerCreateObject error=%s", errorStr);
		  break;
		}
		pInfo->releaseFunc = (SOReleaseObjectFunction)dlsym(pHandle, "HandlerReleaseObject");
		errorStr = dlerror();
		if(errorStr != NULL){
		  LOG_ERROR("load dll func failed HandlerReleaseObject error=%s", errorStr);
		  break;
		}
		pInfo->initializeFunc = (SOInitializeFunction)dlsym(pHandle, "SOInitialize");
		errorStr = dlerror();
		if(errorStr != NULL){
		  LOG_ERROR("load dll func failed SOInitialize error=%s", errorStr);
		  break;
		}
		pInfo->destroyFunc = (SODestroyFunction)dlsym(pHandle, "SODestroy");
		errorStr = dlerror();
		if(errorStr != NULL){
		  LOG_ERROR("load dll func failed SODestroy error=%s", errorStr);
		  break;
		}
		m_soInfoMap.insert(std::make_pair(name, pInfo));
		LOG_INFO("load SO OK name=%s", name.c_str());
		pInfo->initializeFunc();
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
void HandlerCreator::unloadAllSO(void){
	for(auto &kv : m_soInfoMap){
		SOInformation* pInfo = kv.second;
		unloadSOInformation(pInfo);
	}
	m_soInfoMap.clear();
}
void HandlerCreator::unloadSOInformation(SOInformation* pInfo){
	// find and remove all the so objects
	std::vector<uint32> removeModuleTypes;
	for( auto &kv : m_moduleInfoMap ){
		ModuleInformation* pModuleInfo = kv.second;
		if(pModuleInfo->soName == pInfo->soName){
            removeModuleTypes.push_back(pModuleInfo->moduleType);
		}
	}
	for(auto &moduleType : removeModuleTypes){
		unloadModule(moduleType);
	}
	pInfo->destroyFunc();
	// close the SO file
	dlclose(pInfo->pHandle);
	delete pInfo;
}
Handler* HandlerCreator::createObject(uint32 index, uint32 poolType){
	ModuleInformation* pModuleInfo = NULL;
	SOInformation* pInfo = findMatchInfo(index, poolType, &pModuleInfo);
	if(NULL != pInfo){
		Handler* pHandler = pInfo->createFunc(index, poolType);
		return pHandler;
	}else{
		LOG_ERROR("HandlerCreator::createObject findMatchInfo failed index=%d poolType=%d", index, poolType);
	}
	return NULL;
}
void HandlerCreator::releaseObject(Handler* pObj){
	uint32 index = pObj->getIndex();
	uint32 poolType = pObj->getType();
	ModuleInformation* pModuleInfo = NULL;
	SOInformation* pInfo = findMatchInfo(index, poolType, &pModuleInfo);
	if(NULL != pInfo){
		pInfo->releaseFunc(pObj);
	}else{
		LOG_ERROR("HandlerCreator::releaseObject findMatchInfo failed index=%d poolType=%d", index, poolType);
	}
}
SOInformation* HandlerCreator::findMatchInfo(uint32 index, uint32 poolType, ModuleInformation** ppModuleInfo){
	for(auto &kv : m_moduleInfoMap){
		ModuleInformation* pModuleInfo = kv.second;
		*ppModuleInfo = pModuleInfo;
		if(pModuleInfo->poolType == poolType && index >= pModuleInfo->beginIndex && index <= pModuleInfo->endIndex){
			SOInformationMap::iterator itCur = m_soInfoMap.find(pModuleInfo->soName);
			if(itCur != m_soInfoMap.end()){
				return itCur->second;
			}
		}
	}
	return NULL;
}










NS_HIVE_END
