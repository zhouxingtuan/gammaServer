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
#include "handlercreator.h"
#include "dispatcher.h"

NS_HIVE_BEGIN

void parseIPAndPort(const std::string& ip_port_str, std::string& ip, uint16& port){
	Token::TokenMap ip_port_map;
	Token::split(ip_port_str, ":", ip_port_map);
	for(auto &kv : ip_port_map){
		ip = kv.first;
		port = atoi(kv.second.c_str());
		LOG_DEBUG("ip=%s port=%d", ip.c_str(), port);
		return;
	}
}

void loadConfig(const char* fileName){
	Token::TokenMap config;
	Token::readConfig(fileName, config);
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
	const std::string& 	inner_addr 			= config["inner_addr"];
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
	GlobalSetting::createInstance();
	GlobalSetting::getInstance()->setKey(netkey.c_str());
	GlobalSetting::getInstance()->setPassword(password);
	GlobalSetting::getInstance()->setPublicKey(https_public);
	GlobalSetting::getInstance()->setPrivateKey(https_private);
	GlobalSetting::getInstance()->setAcceptReadFunction(DEFAULT_ACCEPT_INDEX, onAcceptRead);
	GlobalSetting::getInstance()->setAcceptReceivePacketFunction(DEFAULT_ACCEPT_INDEX, onAcceptReceivePacket);
	GlobalSetting::getInstance()->setAcceptDecryptFunction(onAcceptDecrypt);
	GlobalSetting::getInstance()->setAcceptEncryptFunction(onAcceptEncrypt);
	GlobalSetting::getInstance()->setReceiveHttpFunction(onReceiveHttp);
	GlobalSetting::getInstance()->setRemoveHttpFunction(onRemoveHttp);
	GlobalSetting::getInstance()->setHttpReceivePacketFunction(onHttpReceivePacket);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_DISPATCH_BY_HANDLE, onCommandDispatchByHandle);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_PING, onCommandPing);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_PONG, onCommandPong);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_REGISTER, onCommandRegister);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_RESPONSE, onCommandResponse);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_HIVE_REGISTER, onCommandHiveRegister);
	GlobalSetting::getInstance()->setAcceptCommandFunction(COMMAND_HIVE_RESPONSE, onCommandHiveResponse);
	MainWorker::getInstance()->initialize((uint32)node_id, (uint32)epoll_number, (uint32)worker_number);

	// create main handler; HANDLER_TYPE_MAIN included
	for(uint32 poolType=0; poolType<DESTINATION_MAX_GROUP; ++poolType){
		GlobalHandler::getInstance()->createPool(poolType, HandlerCreatorCreateObject, HandlerCreatorReleaseObject);
	}

	// create MainHandler
	uint32 mainHandle = GlobalHandler::getInstance()->createDestination(HANDLER_TYPE_MAIN, MAIN_HANDLER_INDEX, "");
	MainHandler* pMain = GlobalHandler::getInstance()->getDestination<MainHandler>(mainHandle);
	LOG_DEBUG("MainHandler handle=%d mainHandle=%d", pMain->getHandle(), mainHandle);

	// create Handlers
	HandlerCreator::createInstance()->initializeSO(config);

	// load command Name->Number
	std::string command_hash_ = "command_hash_";
	for(auto &kv : config){
		const std::string& key = kv.first;
		if( Token::startWith(key, command_hash_) ){
			std::string commandName = key.substr(command_hash_.length());
			uint32 commandNumber = atoi(kv.second.c_str());
			LOG_DEBUG("addCommandMap commandName=%s commandNumber=%d", commandName.c_str(), commandNumber);
			GlobalSetting::getInstance()->addCommandMap(commandName, commandNumber);
		}
	}

	// record main init data
	pMain->m_destID = des_id;
	parseIPAndPort(des_addr, pMain->m_destIP, pMain->m_destPort);
	pMain->m_destEncrypt = (des_encrypt > 0);
	pMain->m_destDecrypt = (des_decrypt > 0);

	parseIPAndPort(inner_addr, pMain->m_innerIP, pMain->m_innerPort);
	pMain->m_innerEncrypt = (inner_encrypt > 0);
	pMain->m_innerDecrypt = (inner_decrypt > 0);

	parseIPAndPort(socket_addr, pMain->m_socketIP, pMain->m_socketPort);
	pMain->m_socketEncrypt = (socket_encrypt > 0);
	pMain->m_socketDecrypt = (socket_decrypt > 0);

	parseIPAndPort(http_addr, pMain->m_httpIP, pMain->m_httpPort);

	parseIPAndPort(https_addr, pMain->m_httpsIP, pMain->m_httpsPort);

	Dispatcher::getInstance()->appendCommandListener(COMMAND_HIVE_REGISTER, mainHandle);

	// kick start main handler
	StartMainHandlerTask* pTask = new StartMainHandlerTask();
	pTask->retain();
	GlobalHandler::getInstance()->dispatchTask(mainHandle, pTask);
	pTask->release();
}

NS_HIVE_END
