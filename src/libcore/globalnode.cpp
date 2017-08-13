//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/27
// Time: 下午10:32
// To change this template use File | Settings | File Templates.
//

#include "globalnode.h"

NS_HIVE_BEGIN

GlobalNode::GlobalNode(void) : RefObject(), Sync() {

}
GlobalNode::~GlobalNode(void){
	GlobalNode::destroy();
}

static GlobalNode* g_pGlobalNode = NULL;
GlobalNode* GlobalNode::getInstance(void){
	return g_pGlobalNode;
}
GlobalNode* GlobalNode::createInstance(void){
	if(g_pGlobalNode == NULL){
		g_pGlobalNode = new GlobalNode();
		g_pGlobalNode->initialize();
		SAFE_RETAIN(g_pGlobalNode)
	}
	return g_pGlobalNode;
}
void GlobalNode::destroyInstance(void){
    SAFE_RELEASE(g_pGlobalNode)
}

void GlobalNode::initialize(void){
	m_nodeConnects.resize(MAX_NODE_NUMBER + 1, 0);
}
void GlobalNode::destroy(void){
	m_nodeConnects.clear();
}

NS_HIVE_END
