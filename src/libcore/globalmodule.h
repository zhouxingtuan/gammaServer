//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/30
// Time: 下午11:01
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__globalmodule__
#define __hive__globalmodule__

#include "handler.h"
#include "globalsetting.h"

NS_HIVE_BEGIN

typedef struct ModuleHandlerInformation{
	uint32 moduleType;
	uint32 moduleIndex;
	uint32 handle;
	uint32 version;
	ModuleHandlerInformation(void) : moduleType(0), moduleIndex(0), handle(0), version(0) {}
}ModuleHandlerInformation;

typedef std::vector<ModuleHandlerInformation> ModuleHandlerInformationVector;   // index数组
typedef std::map<uint32, uint32> ModuleIndexHandleMap;
typedef struct ModuleHandlerInformationGroup{
	ModuleHandlerInformationVector infoVec; // 所有服务信息
	ModuleIndexHandleMap localMap;          // 当前进程的服务信息
}ModuleHandlerInformationGroup;
typedef std::vector<ModuleHandlerInformationGroup> ModuleTypeVector;           // type数组

class GlobalModule : public RefObject, public Sync
{
public:
	ModuleTypeVector m_modules;
public:
	GlobalModule(void);
	virtual ~GlobalModule(void);

	static GlobalModule* getInstance(void);
	static GlobalModule* createInstance(void);
	static void destroyInstance(void);

	bool dispatchRandom(Packet* pPacket, uint32 moduleType);
	bool dispatch(Packet* pPacket, uint32 moduleType, uint32 moduleIndex);
	bool dispatchByHandle(Packet* pPacket, uint32 handle);

	void copyModules(ModuleTypeVector& modules){
		lock();
		modules = m_modules;
		unlock();
	}
	uint32 getModuleSize(uint32 moduleType){
		uint32 size = 0;
		lock();
		if( moduleType < (uint32)m_modules.size() ){
			ModuleHandlerInformationVector& infoVec = m_modules[moduleType].infoVec;
			size = infoVec.size();
		}
		unlock();
		return size;
	}
	uint32 randModuleHandle(uint32 moduleType){
		uint32 handle = 0;
		uint32 moduleIndex;
		lock();
		// 优先使用当前的服务处理，如果没有当前服务，再使用所有服务随机
		if( moduleType < (uint32)m_modules.size() ){
			ModuleHandlerInformationGroup& group = m_modules[moduleType];
			if(group.localMap.size() == 0){
				ModuleHandlerInformationVector& infoVec = group.infoVec;
                if(infoVec.size() > 0){
                    moduleIndex = rand() % infoVec.size();
                    handle = infoVec[moduleIndex].handle;
                }
			}else{
				moduleIndex = rand() % group.localMap.size();
				ModuleIndexHandleMap::iterator itCur = group.localMap.begin();
				for(uint32 i=0; i<moduleIndex; ++i){
					++itCur;
				}
				handle = itCur->second;
			}
		}
		unlock();
        return handle;
	}
	uint32 getModuleHandle(uint32 moduleType, uint32 moduleIndex){
		uint32 handle = 0;
		lock();
		if( moduleType < (uint32)m_modules.size() ){
			ModuleHandlerInformationVector& infoVec = m_modules[moduleType].infoVec;
			if( moduleIndex < (uint32)infoVec.size() ){
				ModuleHandlerInformation& info = infoVec[moduleIndex];
				handle = info.handle;
			}
		}
		unlock();
		return handle;
	}
	bool addInformation(uint32 moduleType, uint32 moduleIndex, uint32 handle, uint32 version){
		bool result = false;
		lock();
		if( moduleType >= (uint32)m_modules.size() ){
			m_modules.resize(moduleType + 1);
		}
		ModuleHandlerInformationGroup& group = m_modules[moduleType];
		ModuleHandlerInformationVector& infoVec = group.infoVec;
		if( moduleIndex >= (uint32)infoVec.size() ){
			infoVec.resize(moduleIndex + 1);
		}
		ModuleHandlerInformation& info = infoVec[moduleIndex];
		if( version > info.version ){
			info.moduleType = moduleType;
			info.moduleIndex = moduleIndex;
			info.handle = handle;
			info.version = version;
			result = true;
			DestinationHandle h(handle);
            uint32 nodeID = GlobalSetting::getInstance()->getNodeID();
            // 当前节点需要加入localMap
            if( h.node == nodeID ){
				group.localMap[moduleIndex] = handle;
            }
		}
		unlock();
		return result;
	}
	bool removeInformation(uint32 moduleType, uint32 moduleIndex){
		bool result = false;
		lock();
		if( moduleType < (uint32)m_modules.size() ){
			ModuleHandlerInformationGroup& group = m_modules[moduleType];
			ModuleHandlerInformationVector& infoVec = group.infoVec;
			if( moduleIndex < (uint32)infoVec.size() ){
				ModuleHandlerInformation& info = infoVec[moduleIndex];
                memset(&info, 0, sizeof(ModuleHandlerInformation));
                result = true;
                ModuleIndexHandleMap::iterator itCur = group.localMap.find(moduleIndex);
                if(itCur != group.localMap.end()){
                    group.localMap.erase(itCur);
                }
			}
		}
		unlock();
		return result;
	}

	void initialize(void);
	void destroy(void);
};

NS_HIVE_END

#endif
