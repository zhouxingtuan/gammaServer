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

#define COMMAND_DISPATCH_BY_HANDLE 6
#define COMMAND_DISPATCH_BY_COMMAND 7

Handler* HandlerCreatorCreateObject(uint32 index);

void HandlerCreatorReleaseObject(Handler* pObj);

typedef HandlerDestinationGroup::CreateFunction SOCreateObjectFunction;
typedef HandlerDestinationGroup::DestroyFunction SOReleaseObjectFunction;
typedef void(*SOInitializeFunction)(void);
typedef void(*SODestroyFunction)(void);

typedef struct SOInformation{
	void* pHandle;
	SOCreateObjectFunction createFunc;
	SOReleaseObjectFunction releaseFunc;
	SOInitializeFunction initializeFunc;
	SODestroyFunction destroyFunc;
	uint32 beginIndex;
	uint32 endIndex;
}SOInformation;

class HandlerCreator : public RefObject, public Sync
{
public:
	typedef std::map<std::string, SOInformation*> SOInformationMap;
	typedef std::vector<SOInformation*> HandlerIndexToSO;

	SOInformationMap m_soInfoMap;
	HandlerIndexToSO m_handlerIndexToSO;
public:
	HandlerCreator(void);
	virtual ~HandlerCreator(void);

	static HandlerCreator* getInstance(void);
	static HandlerCreator* createInstance(void);
	static void destroyInstance(void);

	void initializeSO(const Token::TokenMap& config);
	void loadSO(const std::string& name, int dispType, uint32 poolType, uint32 beginIndex, uint32 endIndex);
	void unloadSO(const std::string& name);
	Handler* CreateObject(uint32 index);
};

NS_HIVE_END

#endif
