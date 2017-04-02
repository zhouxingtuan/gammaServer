//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/2
// Time: 下午3:25
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__globalsetting__
#define __hive__globalsetting__

#include "accept.h"
#include "http.h"

NS_HIVE_BEGIN

#define NET_KEY_LENGTH 16
#define COMMAND_NUMBER 256

typedef void (*AcceptReadFunction)(Accept* pAccept, char* recvBuffer, int nread);
typedef void (*AcceptReceivePacketFunction)(Accept* pAccept, Packet* pPacket);
typedef void (*AcceptEncryptFunction)(Accept* pAccept, Packet* pPacket);
typedef void (*AcceptDecryptFunction)(Accept* pAccept, Packet* pPacket);
typedef void (*ReceiveHttpFunction)(Http* pHttp);
typedef void (*RemoveHttpFunction)(Http* pHttp);
typedef void (*HttpReceivePacketFunction)(Http* pHttp, Packet* pPacket);
typedef void (*AcceptCommandFunction)(Accept* pAccept, Packet* pPacket, uint32 command);

class GlobalSetting : public RefObject
{
public:
	AcceptEncryptFunction m_onAcceptEncrypt;
	AcceptDecryptFunction m_onAcceptDecrypt;
	ReceiveHttpFunction m_onReceiveHttp;
	RemoveHttpFunction m_onRemoveHttp;
	HttpReceivePacketFunction m_onHttpReceivePacket;
	AcceptCommandFunction m_commandArr[COMMAND_NUMBER];
	AcceptReadFunction m_acceptReadArr[COMMAND_NUMBER];
	AcceptReceivePacketFunction m_acceptReceiveArr[COMMAND_NUMBER];
	char m_key[NET_KEY_LENGTH];					// 密钥
	uint32 m_nodeID;							// 节点的ID
	std::string m_password;						// 服务间连接验证密码
public:
	GlobalSetting(void);
	virtual ~GlobalSetting(void);

	static GlobalSetting* getInstance(void);
	static GlobalSetting* createInstance(void);
	static void destroyInstance(void);

	void initialize(uint32 nodeID);
	void destroy(void);

	inline void setAcceptReadFunction(uint8 acceptIndex, AcceptReadFunction func){
		m_acceptReadArr[acceptIndex] = func;
	}
	inline AcceptReadFunction getAcceptReadFunction(uint8 acceptIndex){
		return m_acceptReadArr[acceptIndex];
	}
	inline void setAcceptReceivePacketFunction(uint8 acceptIndex, AcceptReceivePacketFunction func){
		m_acceptReceiveArr[acceptIndex] = func;
	}
	inline AcceptReceivePacketFunction getAcceptReceivePacketFunction(uint8 acceptIndex){
		return m_acceptReceiveArr[acceptIndex];
	}
	inline void setAcceptEncryptFunction(AcceptEncryptFunction func){ m_onAcceptEncrypt = func; }
	inline AcceptEncryptFunction getAcceptEncryptFunction(void){ return m_onAcceptEncrypt; }
	inline void setAcceptDecryptFunction(AcceptDecryptFunction func){ m_onAcceptDecrypt = func; }
	inline AcceptDecryptFunction getAcceptDecryptFunction(void){ return m_onAcceptDecrypt; }
	inline void setReceiveHttpFunction(ReceiveHttpFunction func){ m_onReceiveHttp = func; }
	inline ReceiveHttpFunction getReceiveHttpFunction(void){ return m_onReceiveHttp; }
	inline void setRemoveHttpFunction(RemoveHttpFunction func){ m_onRemoveHttp = func; }
	inline RemoveHttpFunction getRemoveHttpFunction(void){ return m_onRemoveHttp; }
	inline void setHttpReceivePacketFunction(HttpReceivePacketFunction func){ m_onHttpReceivePacket = func; }
	inline HttpReceivePacketFunction getHttpReceivePacketFunction(void){ return m_onHttpReceivePacket; }
	inline void setAcceptCommandFunction(uint8 command, AcceptCommandFunction func){
		m_commandArr[command] = func;
	}
	inline AcceptCommandFunction getAcceptCommandFunction(uint8 command){
		return m_commandArr[command];
	}
	inline bool setKey(const char* key){
		if(strlen(key) < NET_KEY_LENGTH){
			return false;
		}
		memcpy(m_key, key, NET_KEY_LENGTH);
		return true;
	}
	inline const char* getKey(void) const { return m_key; }
	inline void setPassword(const std::string& pwd){ m_password = pwd; }
	inline const std::string& getPassword(void) const { return m_password; }
	inline uint32 getNodeID(void){ return m_nodeID; }
	inline void setNodeID(uint32 nodeID){ m_nodeID = nodeID; }
public:

};

NS_HIVE_END

#endif
