//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/26
// Time: 下午4:48
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__epollhandler__
#define __hive__epollhandler__

#include "core.h"

NS_HIVE_BEGIN

static void onAcceptRead(Accept* pAccept, char* recvBuffer, int nread);
static void onReceiveHttp(Http* pHttp);
static void onRemoveHttp(Http* pHttp);
static void onHttpReceivePacket(Http* pHttp, Packet* pPacket);
static void onCommandPing(Accept* pAccept, Packet* pPacket, uint32 command);
static void onCommandPong(Accept* pAccept, Packet* pPacket, uint32 command);
static void onCommandRegister(Accept* pAccept, Packet* pPacket, uint32 command);
static void onCommandResponse(Accept* pAccept, Packet* pPacket, uint32 command);
static void onCommandHiveRegister(Accept* pAccept, Packet* pPacket, uint32 command);
static void onCommandHiveResponse(Accept* pAccept, Packet* pPacket, uint32 command);

NS_HIVE_END

#endif
