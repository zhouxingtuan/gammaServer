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

void onAcceptRead(Accept* pAccept, char* recvBuffer, int nread);
void onReceiveHttp(Http* pHttp);
void onRemoveHttp(Http* pHttp);
void onHttpReceivePacket(Http* pHttp, Packet* pPacket);
void onCommandPing(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandPong(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandRegister(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandResponse(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandHiveRegister(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandHiveResponse(Accept* pAccept, Packet* pPacket, uint32 command);

NS_HIVE_END

#endif
