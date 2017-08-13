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

// these functions will be called from EpollWorker, work in different threads

void onAcceptRead(Accept* pAccept, char* recvBuffer, int nread);
void onAcceptReceivePacket(Accept* pAccept, Packet* pPacket);
void onAcceptDecrypt(Accept* pAccept, Packet* pPacket);
void onAcceptEncrypt(Accept* pAccept, Packet* pPacket);
void onReceiveHttp(Http* pHttp);
void onRemoveHttp(Http* pHttp);
void onHttpReceivePacket(Http* pHttp, Packet* pPacket);
void onCommandPing(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandPong(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandRegister(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandResponse(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandHiveRegister(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandHiveResponse(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandDispatchByHandle(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandDispatchTransfer(Accept* pAccept, Packet* pPacket, uint32 command);
void onCommandResponseTransfer(Accept* pAccept, Packet* pPacket, uint32 command);

NS_HIVE_END

#endif
