//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/27
// Time: 下午10:25
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__globalnode__
#define __hive__globalnode__

#include "handler.h"

NS_HIVE_BEGIN

#define MAX_NODE_NUMBER 255

class GlobalNode : public RefObject, public Sync
{
public:
	typedef std::vector<uint32> NodeConnectHandle;
public:
	NodeConnectHandle m_nodeConnects;
public:
	GlobalNode(void);
	virtual ~GlobalNode(void);

	static GlobalNode* getInstance(void);
	static GlobalNode* createInstance(void);
	static void destroyInstance(void);

	uint32 getNodeConnect(uint32 nodeID){
		uint32 handle = 0;
		lock();
		if(nodeID < (uint32)m_nodeConnects.size()){
			handle = m_nodeConnects[nodeID];
		}
		unlock();
		return handle;
	}
	bool setNodeConnect(uint32 nodeID, uint32 handle){
		bool result = false;
		lock();
		if(nodeID < (uint32)m_nodeConnects.size()){
			m_nodeConnects[nodeID] = handle;
			result = true;
		}
		unlock();
		return result;
	}
	bool removeNodeConnect(uint32 nodeID){
		return setNodeConnect(nodeID, 0);
	}

	void initialize(void);
	void destroy(void);
};

NS_HIVE_END

#endif
