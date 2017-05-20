//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/8
// Time: 上午11:54
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__handlercreator__
#define __hive__handlercreator__

#include "mainhandler.h"

NS_HIVE_BEGIN

Handler* HandlerCreatorCreateObject(uint32 index, uint32 poolType);

void HandlerCreatorReleaseObject(Handler* pObj);

typedef HandlerDestinationGroup::CreateFunction SOCreateObjectFunction;
typedef HandlerDestinationGroup::DestroyFunction SOReleaseObjectFunction;
typedef void(*SOInitializeFunction)(void);
typedef void(*SODestroyFunction)(void);

typedef struct SOInformation{
	std::string soName;
	void* pHandle;
	SOCreateObjectFunction createFunc;
	SOReleaseObjectFunction releaseFunc;
	SOInitializeFunction initializeFunc;
	SODestroyFunction destroyFunc;
}SOInformation;

typedef struct ModuleInformation{
	std::string soName;
	std::string moduleName;
	uint32 command;
	uint32 poolType;
	uint32 beginIndex;
	uint32 endIndex;
}ModuleInformation;

class HandlerCreator : public RefObject, public Sync
{
public:
	typedef std::map<std::string, SOInformation*> SOInformationMap;
	typedef std::map<std::string, ModuleInformation*> ModuleInformationMap;

	SOInformationMap m_soInfoMap;
	ModuleInformationMap m_moduleInfoMap;
public:
	HandlerCreator(void);
	virtual ~HandlerCreator(void);

	static HandlerCreator* getInstance(void);
	static HandlerCreator* createInstance(void);
	static void destroyInstance(void);

	void initializeSO(Token::TokenMap& config);
	void loadModule(const std::string& soName, const std::string& moduleName, const std::string& param,
		uint32 command, uint32 poolType, uint32 beginIndex, uint32 endIndex);
	void unloadModule(const std::string& moduleName);
	void unloadModuleInformation(ModuleInformation* pModuleInfo);
	void loadSO(const std::string& name);
	void unloadSO(const std::string& name);
	void unloadSOInformation(SOInformation* pInfo);
	void unloadAllSO(void);
	Handler* createObject(uint32 index, uint32 poolType);
	void releaseObject(Handler* pObj);
	SOInformation* findMatchInfo(uint32 index, uint32 poolType, ModuleInformation** ppModuleInfo);
};

NS_HIVE_END

#endif
