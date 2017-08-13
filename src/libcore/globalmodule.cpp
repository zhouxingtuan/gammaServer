//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/30
// Time: 下午11:01
// To change this template use File | Settings | File Templates.
//

#include "globalmodule.h"
#include "globalservice.h"

NS_HIVE_BEGIN

GlobalModule::GlobalModule(void) : RefObject(), Sync() {

}
GlobalModule::~GlobalModule(void){
	GlobalModule::destroy();
}

static GlobalModule* g_pGlobalModule = NULL;
GlobalModule* GlobalModule::getInstance(void){
	return g_pGlobalModule;
}
GlobalModule* GlobalModule::createInstance(void){
	if(g_pGlobalModule == NULL){
		g_pGlobalModule = new GlobalModule();
		g_pGlobalModule->initialize();
		SAFE_RETAIN(g_pGlobalModule)
	}
	return g_pGlobalModule;
}
void GlobalModule::destroyInstance(void){
    SAFE_RELEASE(g_pGlobalModule)
}

bool GlobalModule::dispatchRandom(Packet* pPacket, uint32 moduleType){
	// 优先使用本进程的服务处理
	uint32 handle = randModuleHandle(moduleType);
	if(handle == 0){
		LOG_ERROR("GlobalModule::dispatchRandom handle==0 moduleType=%d", moduleType);
		return false;
	}
	return dispatchByHandle(pPacket, handle);
}
bool GlobalModule::dispatch(Packet* pPacket, uint32 moduleType, uint32 moduleIndex){
	uint32 handle = getModuleHandle(moduleType, moduleIndex);
	if(handle == 0){
        LOG_ERROR("GlobalModule::dispatch handle==0 moduleType=%d moduleIndex=%d", moduleType, moduleIndex);
        return false;
    }
	return dispatchByHandle(pPacket, handle);
}
bool GlobalModule::dispatchByHandle(Packet* pPacket, uint32 handle){
	if(handle == 0){
		LOG_ERROR("GlobalModule::dispatchByHandle handle==0");
		return false;
	}
	DestinationHandle h(handle);
	uint32 nodeID = GlobalSetting::getInstance()->getNodeID();
	// 如果不是当前节点，触发packet转发操作
	if( h.node != nodeID ){
		LOG_DEBUG("COMMAND_DISPATCH_TRANSFER to handle=%d", handle);
		pPacket->setCommand(COMMAND_DISPATCH_TRANSFER);
		pPacket->appendWrite(&handle, sizeof(uint32));
		pPacket->writeEnd();
	}
	return GlobalService::getInstance()->dispatchToService(handle, pPacket);   // 分发使用目的地的handle进行分发
}

void GlobalModule::initialize(void){

}
void GlobalModule::destroy(void){

}

NS_HIVE_END
