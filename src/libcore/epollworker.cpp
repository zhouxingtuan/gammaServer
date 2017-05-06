//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/2/26
// Time: 下午8:53
// To change this template use File | Settings | File Templates.
//

#include "epollworker.h"
#include "globalsetting.h"

NS_HIVE_BEGIN

EpollWorker::EpollWorker(uint32 serviceID) : ActiveWorker(serviceID), Thread(),
 	m_pGroup(NULL), m_pMultiCurl(NULL), m_pPingPacket(NULL), m_pSSLCTX(NULL) {

}
EpollWorker::~EpollWorker(void){
	EpollWorker::destroy();
}

int EpollWorker::threadFunction(void){
	initialize();
	this->retain();
	LOG_INFO("start serviceID=%d", getServiceID());
	int64 timeout;
	while(1){
		timeout = m_pTimer->getWaitTimeout();
		m_pEpoll->update(timeout);
		m_pTimer->update();
	}
	LOG_INFO("exit serviceID=%d", getServiceID());
	EpollWorker::destroy();
	this->release();
	return 0;
}

void EpollWorker::initialize(void){
	// 初始化基类数据
	ActiveWorker::initialize();
	if(NULL == m_pPingPacket){
		m_pPingPacket = new Packet(sizeof(PacketHead));
		m_pPingPacket->retain();
		m_pPingPacket->writeBegin(COMMAND_PING, 0);
		m_pPingPacket->writeEnd();
	}
	if(NULL == m_pGroup){
		uint32 nodeID = GlobalSetting::getInstance()->getNodeID();
		m_pGroup = new EpollObjectDestinationGroup(nodeID, getServiceID());
		m_pGroup->retain();
		m_pGroup->createPool(POOL_TYPE_ACCEPT, Accept::createObject, Accept::releaseObject);
		m_pGroup->createPool(POOL_TYPE_CLIENT, Client::createObject, Client::releaseObject);
		m_pGroup->createPool(POOL_TYPE_HTTP, Http::createObject, Http::releaseObject);
		m_pGroup->createPool(POOL_TYPE_HTTPS, Https::createObject, Https::releaseObject);
	}
	if(NULL == m_pMultiCurl){
		m_pMultiCurl = new MultiCurl();
		m_pMultiCurl->retain();
		m_pMultiCurl->setEpollWorker(this);
		m_pMultiCurl->setEpoll(m_pEpoll);
		m_pMultiCurl->initialize();
	}

	http_parser_settings_init(&m_settings);
	m_settings.on_message_begin 	= Http::onMessageBegin;
	m_settings.on_url 				= Http::onUrl;
	m_settings.on_header_field 		= Http::onHeaderField;
	m_settings.on_header_value 		= Http::onHeaderValue;
	m_settings.on_headers_complete 	= Http::onHeadersComplete;
	m_settings.on_body 				= Http::onBody;
	m_settings.on_message_complete 	= Http::onMessageComplete;
	// 初始化https数据
	/* SSL 库初始化 */
	SSL_library_init();
	/* 载入所有 SSL 算法 */
	OpenSSL_add_all_algorithms();
	/* 载入所有 SSL 错误消息 */
	SSL_load_error_strings();
	/* 以 SSLV2 和 V3 标准兼容方式产生一个 SSL_CTX ,即 SSLContentText*/
	m_pSSLCTX = SSL_CTX_new(SSLv23_server_method());
	/* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
	if (m_pSSLCTX == NULL) {
		ERR_print_errors_fp(stderr);
	}else{
		LOG_INFO("SSL_CTX_new OK");
	}
	const std::string& publicKey = GlobalSetting::getInstance()->getPublicKey();
	const std::string& privateKey = GlobalSetting::getInstance()->getPrivateKey();
	if( !initHttpsCertificate(publicKey.c_str(), privateKey.c_str()) ){
		LOG_ERROR("eoll init https key error service=%d", getServiceID());
	}
}
void EpollWorker::destroy(void){
	SAFE_RELEASE(m_pGroup)
	SAFE_RELEASE(m_pMultiCurl)
	SAFE_RELEASE(m_pPingPacket)
	/* 释放 CTX */
	if(NULL != m_pSSLCTX){
		SSL_CTX_free(m_pSSLCTX);
		m_pSSLCTX = NULL;
	}
}
bool EpollWorker::initHttpsCertificate(const char* publicKey, const char* privateKey){
	if (m_pSSLCTX == NULL) {
		LOG_ERROR("m_pSSLCTX == NULL");
		return false;
	}
	/* 载入用户的数字证书, 此证书用来发送给客户端。 证书里包含有公钥 */
	if (SSL_CTX_use_certificate_file(m_pSSLCTX, publicKey, SSL_FILETYPE_PEM) <= 0) {
    	ERR_print_errors_fp(stderr);
    	return false;
    }
    /* 载入用户私钥 */
    if (SSL_CTX_use_PrivateKey_file(m_pSSLCTX, privateKey, SSL_FILETYPE_PEM) <= 0) {
    	ERR_print_errors_fp(stderr);
    	return false;
    }
    /* 检查用户私钥是否正确 */
    if (!SSL_CTX_check_private_key(m_pSSLCTX)) {
    	ERR_print_errors_fp(stderr);
    	return false;
    }
    return true;
}

bool EpollWorker::dispatchToConnect(uint32 handle, Packet* pPacket){
	// 这里需要生成Task，派发到Epoll线程的里面来执行
	DispatchToConnectTask* pTask = new DispatchToConnectTask();
	pTask->retain();
	pTask->setPacket(pPacket);
	pTask->setDestinationHandle(handle);
	this->acceptTask(pTask);
	pTask->release();
	return true;
}

int64 EpollWorker::checkConnectIdentify(Accept* pAccept){
	if(pAccept->getConnectionState() < CS_IDENTIFY_OK){
		LOG_ERROR("Accept is not CS_IDENTIFY_OK and remove");
		pAccept->epollRemove();
	}else{
		pAccept->setOnline(true);
		Timer* pTimer = (pAccept->getEpollWorker()->getTimer());
		pAccept->setTimeout(CONNECT_ONLINE_TIME, pTimer, EpollWorker::checkConnectOnline);
	}
	return -1;
}
int64 EpollWorker::checkConnectOnline(Accept* pAccept){
	if(!pAccept->isOnline()){
		LOG_ERROR("Accept is not online and remove handle=%d", pAccept->getHandle());
		pAccept->epollRemove();
		return -1;
	}
	pAccept->setOnline(false);
	return CONNECT_ONLINE_TIME;
}
int64 EpollWorker::keepConnectOnline(Accept* pAccept){
//	fprintf(stderr, "--EpollWorker::keepConnectOnline called handle=%d\n", pAccept->getHandle());
	Packet* pPing = new Packet(pAccept->getEpollWorker()->m_pPingPacket->getBuffer());
	pPing->retain();
	pAccept->sendPacket(pPing);
	pPing->release();
	return CONNECT_KEEP_ONLINE_TIME;
}

uint32 EpollWorker::openAccept(int fd, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex){
	LOG_DEBUG("fd=%d ip=%s port=%d", fd, ip, port);
	// 获取一个连接对象Accept，将对象一并加入到epoll中
	Accept* pAccept = (Accept*)m_pGroup->createDestination(POOL_TYPE_ACCEPT, 0);
	if(NULL == pAccept){
		close(fd);
		LOG_ERROR("create accept NULL == pAccept");
		return 0;
	}
	uint32 handle = pAccept->getHandle();
	pAccept->setEpollWorker(this);
	pAccept->setEpoll(m_pEpoll);
	pAccept->setSocketFD( fd );
	pAccept->setSocket( ip, port );
	pAccept->setAcceptIndex(acceptIndex);
	if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK) == -1){
		pAccept->closeSocket();
		closeAccept(handle);
		return 0;
	}
	if( !m_pEpoll->objectAdd(pAccept, EPOLLIN) ){
		pAccept->closeSocket();
		closeAccept(handle);
		return 0;
	}
	pAccept->setConnectionState(CS_CONNECT_OK);
	pAccept->setTimeout(CONNECT_IDENTIFY_TIME, m_pTimer, EpollWorker::checkConnectIdentify);
	pAccept->setIsNeedEncrypt(isNeedEncrypt);
	pAccept->setIsNeedDecrypt(isNeedDecrypt);
	// 发送消息，通知有连接到来？
//	m_pConnectObject->onAcceptIn(handle, ip, port);
	LOG_DEBUG("handle=%d fd=%d ip=%s port=%d", handle, fd, ip, port);
	return handle;
}
uint32 EpollWorker::openClient(uint32 bindHandle, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex){
	Client* pClient = (Client*)m_pGroup->createDestination(POOL_TYPE_CLIENT, 0);
	if(NULL == pClient){
		LOG_ERROR("create client NULL == pClient");
		return 0;
	}
	uint32 handle = pClient->getHandle();
	pClient->setEpollWorker(this);
	pClient->setEpoll(m_pEpoll);
	pClient->setSocket(ip, port);
	pClient->setAcceptIndex(acceptIndex);
	pClient->setConnectionState(CS_CONNECT_START);
	if( !pClient->connectServer() ){
		LOG_ERROR("Client::connectServer failed");
		closeClient(handle);
		return 0;
	}
	// 添加当前的socket到epoll中进行监听
	if( !m_pEpoll->objectAdd(pClient, EPOLLIN | EPOLLOUT) ){
		LOG_ERROR("Epoll::objectAdd failed");
		closeClient(handle);
		return 0;
	}
	pClient->setBindHandle(bindHandle);
	pClient->setIsNeedEncrypt(isNeedEncrypt);
	pClient->setIsNeedDecrypt(isNeedDecrypt);
	LOG_DEBUG("openClient handle=%d ip=%s port=%d bindHandle=%d", handle, ip, port, bindHandle);
	return handle;
}
void EpollWorker::receiveClient(Client* pClient){
	OpenClientOKTask* pTask = new OpenClientOKTask();
	pTask->retain();
	pTask->clientHandle = pClient->getHandle();
	uint32 bindHandle = pClient->getBindHandle();
	GlobalHandler::getInstance()->dispatchTask(bindHandle, pTask);
	pTask->release();
}
uint32 EpollWorker::openHttp(int fd, const char* ip, uint16 port){
	Http* pHttp = (Http*)m_pGroup->createDestination(POOL_TYPE_HTTP, 0);
	if(NULL == pHttp){
		LOG_ERROR("create http NULL == pHttp");
		return 0;
	}
	pHttp->initialize();
	pHttp->setEpollWorker(this);
	pHttp->setEpoll(m_pEpoll);
	pHttp->setSocketFD(fd);
	pHttp->setSocket(ip, port);
	pHttp->setState(HTTP_STATE_READ);
	pHttp->setTimer(HTTP_HANDLE_TIMEOUT, getTimer());
	uint32 handle = pHttp->getHandle();
	// 读取数据
	pHttp->epollIn();
	return handle;
}
uint32 EpollWorker::openHttps(int fd, const char* ip, uint16 port){
	Https* pHttps = (Https*)m_pGroup->createDestination(POOL_TYPE_HTTPS, 0);
	if(NULL == pHttps){
		LOG_ERROR("create https NULL == pHttps");
		return 0;
	}
	pHttps->initialize();
	pHttps->setEpollWorker(this);
	pHttps->setEpoll(m_pEpoll);
	pHttps->setSocketFD(fd);
	pHttps->setSocket(ip, port);
	pHttps->setState(HTTP_STATE_READ);
	pHttps->setTimer(HTTP_HANDLE_TIMEOUT, getTimer());
	uint32 handle = pHttps->getHandle();
	if( !pHttps->bindSSL(m_pSSLCTX) ){
		closeHttps(handle);
		LOG_ERROR("bind ssl failed fd=%d", fd);
		return 0;
	}
	// 读取数据
	pHttps->epollIn();
	return handle;
}

bool EpollWorker::closeAccept(uint32 handle){
	Accept* pAccept = this->getDestination<Accept>(handle);
	if(NULL == pAccept){
		LOG_ERROR("can not find Accept handle=%d", handle);
		return false;
	}
	m_pEpoll->objectRemove(pAccept);
	pAccept->resetData();
	m_pGroup->idleDestination(handle);
	LOG_DEBUG("handle=%d", handle);
	return true;
}
bool EpollWorker::closeClient(uint32 handle){
	Client* pClient = this->getDestination<Client>(handle);
	if(NULL == pClient){
		LOG_ERROR("can not find Client handle=%d", handle);
		return false;
	}
	m_pEpoll->objectRemove(pClient);
	pClient->resetData();
	m_pGroup->idleDestination(handle);
	LOG_DEBUG("handle=%d", handle);
	return true;
}
bool EpollWorker::closeHttp(uint32 handle){
	Http* pHttp = this->getDestination<Http>(handle);
	if(NULL == pHttp){
		LOG_ERROR("can not find Http handle=%d", handle);
		return false;
	}
	pHttp->checkEpollRemove();
	pHttp->setState(HTTP_STATE_DESTROY);
	pHttp->closeSocket();
	m_pGroup->idleDestination(handle);
	return true;
}
bool EpollWorker::closeHttps(uint32 handle){
	return this->closeHttp(handle);
}
bool EpollWorker::closeConnect(uint32 handle){
	DestinationHandle h(handle);
	switch(h.type){
		case POOL_TYPE_ACCEPT:{
			return this->closeAccept(handle);
		}
		case POOL_TYPE_CLIENT:{
			return this->closeClient(handle);
		}
		case POOL_TYPE_HTTP:{
			return this->closeHttp(handle);
		}
		case POOL_TYPE_HTTPS:{
			return this->closeHttps(handle);
		}
		default:{
			return false;
		}
	}
	return false;
}
void EpollWorker::notifyCloseConnect(Accept* pAccept){
	uint32 bindHandle = pAccept->getBindHandle();
	LOG_DEBUG("connection close handle=%d bindHandle=%d", pAccept->getHandle(), bindHandle);
	if(bindHandle == 0){
		LOG_DEBUG("can not find bindHandle %d", bindHandle);
		return;
	}
	CloseConnectTask* pTask = new CloseConnectTask();
	pTask->retain();
	pTask->m_callbackID = 0;
	pTask->m_bindHandle = bindHandle;
	pTask->m_connectHandle = pAccept->getHandle();
	LOG_DEBUG("notifyCloseConnect to bindHandle=%d", bindHandle);
	GlobalHandler::getInstance()->dispatchTask(bindHandle, pTask);
	pTask->release();
}

void EpollWorker::onCurlResponse(Buffer* pBuffer, uint32 callbackID, bool isRequestOK){
	LOG_DEBUG("callback=%d isRequestOK=%d", callbackID, (int)isRequestOK);

}

NS_HIVE_END
