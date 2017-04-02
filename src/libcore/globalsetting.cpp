//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/2
// Time: 下午3:26
// To change this template use File | Settings | File Templates.
//

#include "globalsetting.h"

NS_HIVE_BEGIN

GlobalSetting::GlobalSetting(void) : RefObject(),
	m_onAcceptEncrypt(NULL), m_onAcceptDecrypt(NULL),
	m_onReceiveHttp(NULL), m_onRemoveHttp(NULL), m_onHttpReceivePacket(NULL),
	m_nodeID(0)
{
	for(int i=0; i<COMMAND_NUMBER; ++i){
		m_commandArr[i] = NULL;
		m_acceptReadArr[i] = NULL;
		m_acceptReceiveArr[i] = NULL;
	}
	memset(m_key, 0, sizeof(m_key));
}
GlobalSetting::~GlobalSetting(void){
	GlobalSetting::destroy();
}

static GlobalSetting* g_pGlobalSetting = NULL;
GlobalSetting* GlobalSetting::getInstance(void){
	return g_pGlobalSetting;
}
GlobalSetting* GlobalSetting::createInstance(void){
	if(g_pGlobalSetting == NULL){
		g_pGlobalSetting = new GlobalSetting();
		SAFE_RETAIN(g_pGlobalSetting)
	}
	return g_pGlobalSetting;
}
void GlobalSetting::destroyInstance(void){
    SAFE_RELEASE(g_pGlobalSetting)
}

void GlobalSetting::initialize(uint32 nodeID){
	m_nodeID = nodeID;
}
void GlobalSetting::destroy(void){
	m_onAcceptEncrypt = NULL;
	m_onAcceptDecrypt = NULL;
	m_onReceiveHttp = NULL;
	m_onRemoveHttp = NULL;
	m_onHttpReceivePacket = NULL;
	m_nodeID = 0;
	for(int i=0; i<COMMAND_NUMBER; ++i){
		m_commandArr[i] = NULL;
		m_acceptReadArr[i] = NULL;
		m_acceptReceiveArr[i] = NULL;
	}
	memset(m_key, sizeof(m_key), 0);
}

NS_HIVE_END
