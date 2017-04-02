//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/2
// Time: 下午4:57
// To change this template use File | Settings | File Templates.
//

#include "configloader.h"
#include "epollhandler.h"
#include "mainhandler.h"

NS_HIVE_BEGIN

void loadConfig(const char* fileName){
	Token::TokenMap config;
	Token::readConfig("config.ini", config);
	// # the discovery node to join
	int 				des_id 				= atoi(config["des_id"].c_str());
	const std::string&	des_addr 			= config["des_addr"];
	int 				des_encrypt 		= atoi(config["des_encrypt"].c_str());
	int 				des_decrypt 		= atoi(config["des_decrypt"].c_str());

	// # the node id of current service (1-255)
	int 				node_id 			= atoi(config["node_id"].c_str());
	int 				epoll_number 		= atoi(config["epoll_number"].c_str());
	int 				worker_number 		= atoi(config["worker_number"].c_str());
	const std::string& 	netkey 				= config["netkey"];
	const std::string& 	password 			= config["password"];

	// # inner ip and port to connect between node
	const std::string& 	inner_addr 			= config["netkey"];
	int 				inner_encrypt 		= atoi(config["inner_encrypt"].c_str());
	int 				inner_decrypt 		= atoi(config["inner_decrypt"].c_str());

	// # listen socket ip and port for outer connect; start with 'socket'
	const std::string& 	socket_addr 		= config["socket_addr"];
	int 				socket_encrypt 		= atoi(config["socket_encrypt"].c_str());
	int 				socket_decrypt 		= atoi(config["socket_decrypt"].c_str());

	// # listen http ip and port; start with 'http'
	const std::string& 	http_addr 			= config["http_addr"];

	// # listen https ip and port; start with 'https'
	const std::string& 	https_addr 			= config["https_addr"];
	const std::string& 	https_public 		= config["https_public"];
	const std::string& 	https_private 		= config["https_private"];

	// init the core
	MainWorker::getInstance()->initialize((uint32)node_id, (uint32)epoll_number, (uint32)worker_number);
	GlobalSetting::getInstance()->setKey(netkey.c_str());
	GlobalSetting::getInstance()->setAcceptReadFunction(0, onAcceptRead);
	GlobalSetting::getInstance()->setAcceptReceivePacketFunction(0, onAcceptReceivePacket);
	GlobalSetting::getInstance()->setAcceptDecryptFunction(onAcceptDecrypt);
	GlobalSetting::getInstance()->setAcceptEncryptFunction(onAcceptEncrypt);
	GlobalSetting::getInstance()->setReceiveHttpFunction(onReceiveHttp);
	GlobalSetting::getInstance()->setRemoveHttpFunction(onRemoveHttp);
	GlobalSetting::getInstance()->setHttpReceivePacketFunction(onHttpReceivePacket);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_PING, onCommandPing);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_PONG, onCommandPong);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_REGISTER, onCommandRegister);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_RESPONSE, onCommandResponse);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_HIVE_REGISTER, onCommandHiveRegister);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_HIVE_RESPONSE, onCommandHiveResponse);

	// create main handler
	GlobalHandler::getInstance()->createPool(HANDLER_TYPE_MAIN, MainHandler::createObject, MainHandler::releaseObject);
	uint32 mainHandle = GlobalHandler::getInstance()->createDestination(HANDLER_TYPE_MAIN);
	MainHandler* pMain = GlobalHandler::getInstance()->getDestination<MainHandler>(mainHandle);

	// record main init data

	// kick start main handler
	StartMainHandlerTask* pTask = new StartMainHandlerTask();
	pTask->retain();
	GlobalHandler::getInstance()->dispatchTask(mainHandle, pTask);
	pTask->release();
}

NS_HIVE_END
