//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/27
// Time: 下午10:41
// To change this template use File | Settings | File Templates.
//

#include "globalgroup.h"

NS_HIVE_BEGIN

GlobalGroup::GlobalGroup(void) : RefObject(), Sync() {

}
GlobalGroup::~GlobalGroup(void){
	GlobalGroup::destroy();
}

static GlobalGroup* g_pGlobalGroup = NULL;
GlobalGroup* GlobalGroup::getInstance(void){
	return g_pGlobalGroup;
}
GlobalGroup* GlobalGroup::createInstance(void){
	if(g_pGlobalGroup == NULL){
		g_pGlobalGroup = new GlobalGroup();
		g_pGlobalGroup->initialize();
		SAFE_RETAIN(g_pGlobalGroup)
	}
	return g_pGlobalGroup;
}
void GlobalGroup::destroyInstance(void){
    SAFE_RELEASE(g_pGlobalGroup)
}

bool GlobalGroup::getGroupRoute(const std::string& groupName, const std::string& moduleName, uint32& connectHandle, uint32& command){
	GroupInformationMap::iterator itCur;
	ModuleNameToCommand::iterator cmdCur;
	uint32 handle = 0;
	bool result = false;
	lock();
	do{
		itCur = m_groups.find(groupName);
		if(itCur == m_groups.end()){
			break;
		}
		GroupInformation& info = itCur->second;
		if(info.connectHandle.size() == 0){
			break;
		}
		cmdCur = info.moduleCommand.find(moduleName);
		if(cmdCur == info.moduleCommand.end()){
			break;
		}
		for(uint32 i=info.connectIndex; i<(uint32)info.connectHandle.size(); ++i){
	        handle = info.connectHandle[i];
	        if(handle > 0){
	            info.connectIndex = i + 1;
	            if(info.connectIndex > (uint32)info.connectHandle.size()){
	                info.connectIndex = 0;
	            }
	            break;
	        }
	    }
	    if(handle == 0){
	        for(uint32 i=0; i<info.connectIndex && i<(uint32)info.connectHandle.size(); ++i){
	            handle = info.connectHandle[i];
	            if(handle > 0){
	                info.connectIndex = i + 1;
	                if(info.connectIndex > (uint32)info.connectHandle.size()){
	                    info.connectIndex = 0;
	                }
	                break;
	            }
	        }
	        if(handle == 0){
                break;
            }
	    }
	    result = true;
		connectHandle = handle;
		command = cmdCur->second;
	}while(0);
	unlock();
	return result;
}
bool GlobalGroup::setGroupRoute(const std::string& groupName, uint32 connectHandle){
	GroupInformationMap::iterator itCur;
	bool exited = false;
	lock();
	itCur = m_groups.find(groupName);
    if(itCur != m_groups.end()){
        GroupInformation& info = itCur->second;
        for(auto handle : info.connectHandle){
            if(handle == connectHandle){
                exited = true;
            }
        }
        if( !exited ){
            info.connectHandle.push_back(connectHandle);
        }
    }
	unlock();
	return (!exited);
}
bool GlobalGroup::removeGroupRoute(const std::string& groupName, uint32 connectHandle){
	GroupInformationMap::iterator itCur;
	std::vector<uint32>::iterator handleCur;
	bool result = false;
	lock();
	itCur = m_groups.find(groupName);
    if(itCur != m_groups.end()){
        GroupInformation& info = itCur->second;
		handleCur = info.connectHandle.begin();
		for( ; handleCur!=info.connectHandle.end() ; ++handleCur){
			if( (*handleCur) == connectHandle ){
				info.connectHandle.erase(handleCur);
				result = true;
				break;
			}
		}
    }
	unlock();
	return result;
}
void GlobalGroup::appendGroupModule(const std::string& groupName, const std::string& moduleName, uint32 command){
	GroupInformationMap::iterator itCur;
	lock();
	itCur = m_groups.find(groupName);
	if(itCur == m_groups.end()){
		GroupInformation info;
		info.groupName = groupName;
		info.connectIndex = 0;
		info.moduleCommand.insert(std::make_pair(moduleName, command));
		m_groups.insert(std::make_pair(groupName, info));
	}else{
		GroupInformation& info = itCur->second;
//		info.moduleCommand.insert(std::make_pair(moduleName, command));
		info.moduleCommand[moduleName] = command;
	}
	unlock();
}
void GlobalGroup::addGroup(const std::string& groupName){
	GroupInformationMap::iterator itCur;
	lock();
	itCur = m_groups.find(groupName);
    if(itCur == m_groups.end()){
        GroupInformation info;
        info.groupName = groupName;
        info.connectIndex = 0;
        m_groups.insert(std::make_pair(groupName, info));
    }
	unlock();
}
void GlobalGroup::removeGroup(const std::string& groupName){
	GroupInformationMap::iterator itCur;
	lock();
	itCur = m_groups.find(groupName);
	if(itCur != m_groups.end()){
		m_groups.erase(itCur);
	}
	unlock();
}
void GlobalGroup::removeGroupModule(const std::string& groupName, const std::string& moduleName){
	GroupInformationMap::iterator itCur;
	ModuleNameToCommand::iterator cmdCur;
	lock();
	itCur = m_groups.find(groupName);
	if(itCur != m_groups.end()){
		GroupInformation& info = itCur->second;
		cmdCur = info.moduleCommand.find(moduleName);
		if( cmdCur != info.moduleCommand.end() ){
			info.moduleCommand.erase(cmdCur);
		}
	}
	unlock();
}

void GlobalGroup::initialize(void){

}
void GlobalGroup::destroy(void){

}

NS_HIVE_END
