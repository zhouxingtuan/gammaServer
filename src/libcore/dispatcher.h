//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/1
// Time: 下午8:37
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__dispatcher__
#define __hive__dispatcher__

#include "globalhandler.h"

NS_HIVE_BEGIN

class Dispatcher : public RefObject, public Sync
{
public:
	typedef std::vector<uint32> HandlerVector;
	typedef struct CommandHandlerInfo{
		uint32 index;
		HandlerVector handles;
	}CommandHandlerInfo;
	typedef std::vector<CommandHandlerInfo> CommandHandlerVector;
	CommandHandlerVector m_listeners;
public:
	Dispatcher(void);
	virtual ~Dispatcher(void);

	static Dispatcher* getInstance(void);
	static Dispatcher* createInstance(void);
	static void destroyInstance(void);

	void removeCommandListener(uint32 command, uint32 handle);
	bool appendCommandListener(uint32 command, uint32 handle);
	bool dispatchCommand(Packet* pPacket, uint32 command);
	uint32 getCommandHandle(uint32 command);
};


NS_HIVE_END

#endif
