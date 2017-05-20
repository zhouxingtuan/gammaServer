//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/26
// Time: 下午4:49
// To change this template use File | Settings | File Templates.
//

#include "epollhandler.h"
#include "dispatcher.h"
#include "http.h"
#include "http_parser.h"

NS_HIVE_BEGIN

void onAcceptRead(Accept* pAccept, char* recvBuffer, int nread){
    Packet* pPacket;
    pPacket = pAccept->getTempReadPacket();
    //check stick message
    if( NULL != pPacket ){
    	int needLength = pPacket->getLength() - pPacket->getCursor();
    	if( needLength > nread ){
			// 数据不够，仍然继续等待后续数据
			pPacket->write(recvBuffer, nread);
			return;
    	}else{
			// 数据包已经完整，发送这个消息
			pPacket->write(recvBuffer, needLength);
			// 派发消息给对应的消息处理器
			pAccept->dispatchPacket(pPacket, pPacket->getHead()->command);
			pPacket->release();		// 对应Packet创建时的retain
			pPacket = NULL;
			// 剩下的消息需要继续解析
			if(nread == needLength){
				// 使用完临时数据之后，需要直接清除
				pAccept->setTempReadPacket(pPacket);
				return;
			}
			recvBuffer += needLength;
			nread -= needLength;
    	}
    }
    // 解析剩余的数据包
	char* recvBufferPtr;
	int packetLength;
	int writeLength;
	// 数据量太少，如果发生就直接丢了吧
	if( nread < (int)sizeof(PacketHead) ){
		return;
	}
	//这里读取的信息很可能包含多条信息，这时候需要解析出来；这几条信息因为太短，在发送时被底层socket合并了
	recvBufferPtr = recvBuffer;
	do{
		// 对头部数据进行解密
		if( pAccept->isNeedDecrypt() ){
			LOG_DEBUG("Accept decrypt head");
			binary_decrypt(recvBufferPtr, 8, GlobalSetting::getInstance()->getKey());
		}
		packetLength = ((PacketHead*)(recvBufferPtr))->length;
		if( packetLength < (int)sizeof(PacketHead) || packetLength > pAccept->getMaxLength() ){
			LOG_ERROR("head length is invalid packetLength=%d", packetLength);
			break;	// 这里直接将数据丢弃
		}
		writeLength = std::min( (int)(nread-(recvBufferPtr-recvBuffer)), packetLength );
		// 创建Packet对象，并将数据写入
		pPacket = new Packet(packetLength);
		pPacket->retain();	// 如果数据没有全部收到，那么m_tempReadPacket会保持这个retain状态
		pPacket->write( recvBufferPtr, writeLength );
		recvBufferPtr += writeLength;
		if( pPacket->isCursorEnd() ){
			// 派发消息给对应的消息处理器
			pAccept->dispatchPacket(pPacket, pPacket->getHead()->command);
			pPacket->release();
			pPacket = NULL;
		}
		// 如果消息没有全部接收，那么将会放到临时包中等待下一次读数据操作
	}while(nread-(recvBufferPtr-recvBuffer) > (int)sizeof(PacketHead));
    pAccept->setTempReadPacket(pPacket);
}
void onAcceptReceivePacket(Accept* pAccept, Packet* pPacket){
	LOG_DEBUG("handle=%d packet length=%d", pAccept->getHandle(), pPacket->getLength());
	pAccept->sendPacket(pPacket);
}
void onAcceptDecrypt(Accept* pAccept, Packet* pPacket){
	LOG_DEBUG("handle=%d packet length=%d", pAccept->getHandle(), pPacket->getLength());
	if( pPacket->getLength() > 8 ){
		binary_decrypt(pPacket->getDataPtr()+8, pPacket->getLength()-8, GlobalSetting::getInstance()->getKey());
	}
}
void onAcceptEncrypt(Accept* pAccept, Packet* pPacket){
	LOG_DEBUG("handle=%d packet length=%d", pAccept->getHandle(), pPacket->getLength());
	binary_encrypt(pPacket->getDataPtr(), pPacket->getLength(), GlobalSetting::getInstance()->getKey());
}
void onReceiveHttp(Http* pHttp){
	LOG_DEBUG("handle=%d buffer=\n%s", pHttp->getHandle(), pHttp->getBuffer()->data());
	// if this is a preflight request
	if(HTTP_OPTIONS == pHttp->getParser()->method){
		static char options_response[] = {"HTTP/1.1 200 OK\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: POST,GET\r\n"
                    "Access-Control-Allow-Headers: *\r\n"
                    "Content-Type: text/html; charset=utf-8\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n"
                    ""};
		LOG_DEBUG("options_response length=%d data=\n%s", (int)sizeof(options_response), options_response);
		pHttp->responseRequest(options_response, sizeof(options_response));
		return;
	}
	uint32 command = COMMAND_DISPATCH_BY_HANDLE;
	uint32 handle = 0;
	// find cmd and handle
	Token::TokenVector urlVector;
	std::string url(pHttp->getUrl(), pHttp->getUrlLength());
	Token::splitArray(url, "/", urlVector);
	LOG_DEBUG("url=%s", url.c_str());
	for(auto &s : urlVector){
		LOG_DEBUG("%s", s.c_str());
	}
	if(urlVector.size() == 0){
		LOG_ERROR("can not find command for url");
		static char no_command_response[] = {"HTTP/1.1 200 OK\r\n"
					"Access-Control-Allow-Origin: *\r\n"
	                "Content-Type: text/html; charset=utf-8\r\n"
	                "Content-Length: 40\r\n"
	                "\r\n"
	                "{\"error\":\"can not find command for url\"}"};
		pHttp->responseRequest(no_command_response, sizeof(no_command_response));
		return;
	}else if(urlVector.size() == 1){
		command = GlobalSetting::getInstance()->getCommand(urlVector[0]);
	}else{
		command = GlobalSetting::getInstance()->getCommand(urlVector[0]);
		handle = atoi(urlVector[1].c_str());
	}
	if(command == INVALID_COMMAND){
		LOG_ERROR("INVALID_COMMAND found commandStr=%s", urlVector[0].c_str());
		static char invalid_command_response[] = {"HTTP/1.1 200 OK\r\n"
					"Access-Control-Allow-Origin: *\r\n"
                    "Content-Type: text/html; charset=utf-8\r\n"
                    "Content-Length: 35\r\n"
                    "\r\n"
                    "{\"error\":\"invalid command request\"}"};
		pHttp->responseRequest(invalid_command_response, sizeof(invalid_command_response));
		return;
	}
	LOG_DEBUG("get url commandStr=%s command=%d handle=%d", urlVector[0].c_str(), command, handle);
	// dispatch the packet
	Packet* pPacket = new Packet(sizeof(PacketHead) + pHttp->getBodyLength());
	pPacket->retain();
	pPacket->writeBegin(command, handle);
	pPacket->write(pHttp->getBody(), pHttp->getBodyLength());
	pPacket->writeEnd();
	// 判断cmd执行后续操作
	if(command == COMMAND_DISPATCH_BY_HANDLE){
		LOG_DEBUG("onReceiveHttp handle=%d dispatchToService", pHttp->getHandle());
		GlobalService::getInstance()->dispatchToService(pPacket);
	}else{
		LOG_DEBUG("onReceiveHttp handle=%d dispatchPacket command=%d function not found.", pHttp->getHandle(), command);
		// 这里不执行的命令，发送消息给后面的服务执行
		pPacket->setDestination(pHttp->getHandle());
		Dispatcher::getInstance()->dispatchCommand(pPacket, command);
	}
	pPacket->release();
}
void onRemoveHttp(Http* pHttp){
	LOG_DEBUG("handle=%d", pHttp->getHandle());

}
void onHttpReceivePacket(Http* pHttp, Packet* pPacket){
	LOG_DEBUG("handle=%d packet length=%d", pHttp->getHandle(), pPacket->getLength());
	// here will skip the packet head
	static char ok_response[] = {"HTTP/1.1 200 OK\r\n"
				"Access-Control-Allow-Origin: *\r\n"
                "Content-Type: text/html; charset=utf-8\r\n"
                "Content-Length: %d\r\n"
                "\r\n"};
    char headBuffer[256];
    // 写入头部长度
    int bodyLength = pPacket->getBodyLength();
    sprintf(headBuffer, ok_response, bodyLength);
    pHttp->responseBegin(sizeof(ok_response) + bodyLength);
    pHttp->responseAppend(ok_response, sizeof(ok_response));
    pHttp->responseAppend(pPacket->getBody(), bodyLength);
    pHttp->responseEnd();
}
void onCommandPing(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	pAccept->setOnline(true);
	pPacket->setCommand(COMMAND_PONG);
	pAccept->sendPacket(pPacket);
}
void onCommandPong(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	pAccept->setOnline(true);
}
void onCommandRegister(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	char temp[256] = {0};
	uint32 nodeID = 0;
	uint32 t = 0;
	uint64 magic = 0;
	pPacket->readBegin();
	pPacket->read(&nodeID, sizeof(uint32));
	pPacket->read(&t, sizeof(uint32));
	pPacket->read(&magic, sizeof(uint64));
	pPacket->readEnd();
	const std::string& password = GlobalSetting::getInstance()->getPassword();
	sprintf(temp, "%04d-%d-%s", nodeID, t, password.c_str());
	uint64 magicHere = binary_hash64(temp, strlen(temp));
	LOG_DEBUG("onCommandRegister nodeID=%d str=%s magic=%llu magicHere=%llu", nodeID, temp, magic, magicHere);
	if(magic != magicHere){
		// response error message
		Packet* pResponse = new Packet(PACKET_HEAD_LENGTH);
        pResponse->retain();
        pResponse->writeBegin(COMMAND_RESPONSE, 0);
        pResponse->writeEnd();
        bool result = pAccept->sendPacket(pResponse);
        uint32 connectHandle = pPacket->getDestination();
        LOG_DEBUG("onCommandRegister failed to remote connectHandle=%d result=%d acceptHandle=%d", connectHandle, result, pAccept->getHandle());
        pResponse->release();
		// close the connection
		pAccept->epollRemove();
	}else{
		// response success message
		uint32 acceptHandle = pAccept->getHandle();
		Packet* pResponse = new Packet(PACKET_HEAD_LENGTH);
        pResponse->retain();
        pResponse->writeBegin(COMMAND_RESPONSE, acceptHandle);
        pResponse->writeEnd();
        bool result = pAccept->sendPacket(pResponse);
        uint32 connectHandle = pPacket->getDestination();
	    LOG_DEBUG("onCommandRegister OK to remote connectHandle=%d result=%d acceptHandle=%d", connectHandle, result, acceptHandle);
        pResponse->release();
		// set Accept to identify state
		pAccept->setConnectionState(CS_IDENTIFY_OK);
	}
}
void onCommandResponse(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	uint32 remoteHandle = pPacket->getDestination();
	LOG_DEBUG("onCommandResponse remoteHandle=%d", remoteHandle);
	if(remoteHandle > 0){
		// set Client to identify state
        pAccept->setConnectionState(CS_IDENTIFY_OK);
		// schedule current client to keep online
		Timer* pTimer = (pAccept->getEpollWorker()->getTimer());
		pAccept->setTimeout(CONNECT_KEEP_ONLINE_TIME, pTimer, EpollWorker::keepConnectOnline);
		// identify OK, tell the MainHandler to register Hive
		pPacket->setDestination(pAccept->getHandle());
		uint32 bindHandle = pAccept->getBindHandle();
		if(bindHandle > 0){
			LOG_DEBUG("onCommandResponse dispatch to bindHandle=%d", bindHandle);
			GlobalHandler::getInstance()->dispatchToHandler(pPacket, bindHandle);
		}else{
			LOG_ERROR("command=%d will not be handle bindHandle==0", command);
		}
	}
}
void onCommandHiveRegister(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	pPacket->setDestination(pAccept->getHandle());
	Dispatcher::getInstance()->dispatchCommand(pPacket, command);
}
void onCommandHiveResponse(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);
	pPacket->setDestination(pAccept->getHandle());
	uint32 bindHandle = pAccept->getBindHandle();
	if(bindHandle > 0){
		LOG_DEBUG("onCommandResponse dispatch to bindHandle=%d", bindHandle);
        GlobalHandler::getInstance()->dispatchToHandler(pPacket, bindHandle);
	}else{
		LOG_ERROR("command=%d will not be handle bindHandle==0", command);
	}
}
void onCommandDispatchByHandle(Accept* pAccept, Packet* pPacket, uint32 command){
	GlobalService::getInstance()->dispatchToService(pPacket);
}

NS_HIVE_END
