//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/27
// Time: 下午10:40
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__globalgroup__
#define __hive__globalgroup__

#include "handler.h"

NS_HIVE_BEGIN

typedef std::unordered_map<std::string, uint32> ModuleNameToCommand;
typedef struct GroupInformation{
	std::string groupName;
	std::vector<uint32> connectHandle;
	ModuleNameToCommand moduleCommand;
	uint32 connectIndex;
}GroupInformation;
typedef std::unordered_map<std::string, GroupInformation> GroupInformationMap;

class GlobalGroup : public RefObject, public Sync
{
public:
	GroupInformationMap m_groups;
public:
	GlobalGroup(void);
	virtual ~GlobalGroup(void);

	static GlobalGroup* getInstance(void);
	static GlobalGroup* createInstance(void);
	static void destroyInstance(void);

	void addGroup(const std::string& groupName);
    void removeGroup(const std::string& groupName);
	bool getGroupRoute(const std::string& groupName, const std::string& moduleName, uint32& connectHandle, uint32& command);
	bool setGroupRoute(const std::string& groupName, uint32 connectHandle);
	bool removeGroupRoute(const std::string& groupName, uint32 connectHandle);
	void appendGroupModule(const std::string& groupName, const std::string& moduleName, uint32 command);
	void removeGroupModule(const std::string& groupName, const std::string& moduleName);

	void initialize(void);
	void destroy(void);
};

NS_HIVE_END

#endif
