//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/6/5
// Time: 下午4:00
// To change this template use File | Settings | File Templates.
//

#include <chrono>
#include <thread>
#include <time.h>
#include "client.h"

USING_NS_HIVE;

class ClientNotify : public ClientInterface
{
public:
	ClientNotify(void) : ClientInterface(){}
	virtual ~ClientNotify(void){}

	virtual void notifyConnectServerFailed(Client* pClient){
		fprintf(stderr, "Client called notifyConnectServerFailed\n");
	}
	virtual void notifyConnectServerSuccess(Client* pClient){
		fprintf(stderr, "Client called notifyConnectServerSuccess\n");
//		Packet* p = new Packet(8);
//		int a = 123;
//		p->write(&a, sizeof(int));
//		p->retain();
//		pClient->receivePacket(p);
//		p->release();
	}
	virtual void notifyConnectOut(Client* pClient){
		fprintf(stderr, "Client called notifyConnectOut\n");
		pClient->reconnectSocket();
	}
	virtual void notifyPacketIn(Client* pClient, Packet* pPacket){
		fprintf(stderr, "Client called notifyPacketIn command=%d destination=%d\n", pPacket->getCommand(), pPacket->getDestination());

	}
	virtual void notifyIdentifyServerFailed(Client* pClient, Packet* pPacket){
		fprintf(stderr, "Client identify server Failed\n");

	}
	virtual void notifyIdentifyServerSuccess(Client* pClient, Packet* pPacket){
		unsigned int connectHandle = pPacket->getDestination();
		fprintf(stderr, "Client identify server success connectHandle=%d\n", connectHandle);

	}
};

int main(int argc, char *argv[])
{
	fprintf(stderr, "hello world!\n");

	ClientNotify* pNotify = new ClientNotify();
	Client* pClient = new Client();
	pClient->retain();

	pClient->setNotifyInterface(pNotify);
	pClient->setKey("1234567890123456");
	pClient->setPassword("123456");
	pClient->setIsNeedEncrypt(true);
	pClient->setIsNeedDecrypt(true);
	pClient->setSocket("127.0.0.1", 9902);
	pClient->startThread();		// 这个函数开启新的线程建立连接

	while(true){
		std::chrono::milliseconds timespan(1*1000); // or whatever
		std::this_thread::sleep_for(timespan);
		// 这个函数需要在主循环中调用，用来分发事件；这样就确保调用线程安全
		pClient->dispatchEvent();
	};

	std::chrono::milliseconds timespan(10*1000); // or whatever
	std::this_thread::sleep_for(timespan);

	pClient->release();
	delete pNotify;
	return 0;
}

