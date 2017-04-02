//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/6/5
// Time: 下午3:59
// To change this template use File | Settings | File Templates.
//

#include "client.h"

NS_HIVE_BEGIN

/*--------------------------------------------------------------------*/
bool Thread::startThread(void){
	cancelThread();
	if( pthread_create(&m_pThread,NULL,staticThreadFunction,this) ){
		return false;
	}
	pthread_detach(m_pThread);
	return true;
}
void* Thread::staticThreadFunction(void* pData){
	Thread* ret = (Thread*)pData;
	return (void*)ret->threadFunction();
}
void Thread::cancelThread(void){
	if(m_pThread != 0 ){
		pthread_cancel( m_pThread );
		m_pThread = 0;
	}
}
pthread_t Thread::staticThread(ThreadCallback start_rtn, void *arg){
	pthread_t pThread;
	if( pthread_create(&pThread,NULL,start_rtn,arg) ){
		return 0;
	}
	pthread_detach(pThread);
	return pThread;
}
/*--------------------------------------------------------------------*/
Buffer::Buffer(int length) : RefObject(), CharVector(length, 0) {
	this->clear();
}
Buffer::~Buffer(void){

}
/*--------------------------------------------------------------------*/
Packet::Packet(int bufferSize) : RefObject(),m_pBuffer(NULL),m_cursor(0) {
	assert(bufferSize > 0 && "Packet new bufferSize should be > 0");
	m_pBuffer = new Buffer();	// 是否处理分配内存失败？
	m_pBuffer->retain();
	m_pBuffer->reserve(bufferSize);
}
Packet::Packet(Buffer* pBuffer) : RefObject(),m_pBuffer(pBuffer),m_cursor(0) {
	assert(pBuffer != NULL && "Packet new pBuffer should not be NULL");
	pBuffer->retain();
}
Packet::~Packet(void){
	SAFE_RELEASE(m_pBuffer)
}
/*--------------------------------------------------------------------*/
Client::Client(void) : RefObject(), Sync(), Thread(),
	m_fd(0), m_port(0),
	m_tempReadPacket(NULL), m_pInterface(NULL) {
	memset(m_ip, 0, sizeof(m_ip));
}
Client::~Client(void){
	releasePacket();
	SAFE_RELEASE(m_tempReadPacket)
}
void Client::releasePacket(void){
	this->lock();
	for( auto pPacket : m_packetQueue ){
		pPacket->release();
	}
	m_packetQueue.clear();
	this->unlock();
}
bool Client::receivePacket(Packet* pPacket){
	if( 0 == m_fd ){
		return false;
	}
	pPacket->resetCursor();		// 后面的写操作需要重置
	pPacket->retain();			// 进入队列前引用
	this->lock();
	m_packetQueue.push_back(pPacket);
	this->unlock();
	return true;
}
int Client::threadFunction(void){
	assert(NULL != m_pInterface && "m_pInterface for Client can not be NULL");
	if( !connectServer() ){
		addClientEvent(CLIENT_EVENT_CONN_FAILED, NULL);
		return 0;
	}
	addClientEvent(CLIENT_EVENT_CONN_SUCCESS, NULL);
	while(true){
		if( !trySelectSocket() ){
			fprintf(stderr, "select failed. client out ...\n");
			removeSocket();
			addClientEvent(CLIENT_EVENT_CONN_OUT, NULL);
			return 0;
		}
	};
	return 0;
}
void Client::dispatchPacket(Packet* pPacket){
	addClientEvent(CLIENT_EVENT_PACKET_IN, pPacket);
}
void Client::dispatchEvent(void){
	// 一次性处理完所有的消息
	m_tempEventQueue.clear();
	lock();
	m_tempEventQueue.swap(m_clientEventQueue);
	unlock();
	ClientEvent evt;
	memset(&evt, 0, sizeof(ClientEvent));
	while(!m_tempEventQueue.empty()){
		evt = m_tempEventQueue.front();
		m_tempEventQueue.pop_front();
		switch(evt.event){
    		case CLIENT_EVENT_NONE: break;	// nothing happened
    		case CLIENT_EVENT_PACKET_IN:{
    			m_pInterface->notifyPacketIn(this, evt.pPacket);
    			break;
    		}
    		case CLIENT_EVENT_CONN_FAILED:{
    			m_pInterface->notifyConnectServerFailed(this);	// 通知外部连接失败
    			break;
    		}
    		case CLIENT_EVENT_CONN_SUCCESS:{
    			m_pInterface->notifyConnectServerSuccess(this);	// 通知外部连接成功
    			break;
    		}
    		case CLIENT_EVENT_CONN_OUT:{
    			m_pInterface->notifyConnectOut(this);	// 通知外部连接退出
    			break;
    		}
    		default:{
    			fprintf(stderr, "unknown event type for client\n");
    			break;
    		}
    	};
    	if( NULL != evt.pPacket ){
    		evt.pPacket->release();
    	}
	};

}
void Client::addClientEvent(ClientEventType event, Packet* pPacket){
	ClientEvent evt;
	evt.event = event;
	evt.pPacket = pPacket;
	if( NULL != pPacket ){
		pPacket->retain();
	}
	lock();
	m_clientEventQueue.push_back(evt);
	unlock();
}
bool Client::trySelectSocket(void){
	if( !tryReadSocket() ){
		return false;
	}
	if( !tryWriteSocket() ){
		return false;
	}
	return true;
}
bool Client::tryReadSocket(void){
	timeval	seltime;
	int		iResult = 0;
	seltime.tv_sec = 0;
	seltime.tv_usec = CLIENT_SELECT_TIMEOUT;
	fd_set readSet;
	FD_ZERO( &readSet );
	FD_SET( m_fd, &readSet );
	iResult = select(FD_SETSIZE, &readSet, NULL, NULL, &seltime);
	if( iResult < 0){
		fprintf(stderr,"--Client::tryReadSocket select read state error.\n");
		return false;
	}else if( iResult == 0 ){
//		fprintf(stderr, "select iResult == 0\n");
		return true;
	}
	if( FD_ISSET( m_fd, &readSet ) ){	//read data
		if( !readSocket() ){
			return false;
		}
	}
	return true;
}
bool Client::tryWriteSocket(void){
	timeval	seltime;
	int		iResult = 0;
	seltime.tv_sec = 0;
	seltime.tv_usec = CLIENT_SELECT_TIMEOUT;
	fd_set writeSet;
	FD_ZERO( &writeSet );
	FD_SET( m_fd, &writeSet );
	iResult = select(FD_SETSIZE, NULL, &writeSet, NULL, &seltime);
	if( iResult < 0){
		fprintf(stderr,"--Client::tryWriteSocket select write state error.\n");
		return false;
	}else if( iResult == 0 ){
//		fprintf(stderr, "tryWriteSocket select iResult == 0\n");
		return true;
	}
	if( FD_ISSET( m_fd, &writeSet ) ){//write data
		if( !writePacket() ){
			return false;
		}
	}
	return true;
}
void Client::removeSocket(void){
	closeSocket();	// 关闭套接字
	SAFE_RELEASE(m_tempReadPacket)
	releasePacket();	// 取消所有数据包的发送
}
bool Client::readSocket(void){
	char recvBuffer[8192];
    char* recvBufferPtr;
    int nread;
    int packetLength;
    int writeLength;
    Packet* pPacket;
    pPacket = m_tempReadPacket;
    if( pPacket == NULL ){
        nread = read(m_fd, recvBuffer, 8192);
    }else{
        nread = read(m_fd, pPacket->getCursorPtr(), pPacket->getLength()-pPacket->getCursor());
    }
    if(nread < 0){
        switch(errno){
        case EINTR: return true; // 读数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
            return true;
        default: return false;
        }
        return false;
    }else if(nread == 0){
        return false;
    }
    //check stick message
    if( NULL != pPacket ){
    	pPacket->moveCursor(nread);
    	if( pPacket->isCursorEnd() ){
			// 派发消息给对应的消息处理器
			dispatchPacket(pPacket);
    		pPacket->release();		// 对应Packet创建时的retain
			pPacket = NULL;
    	}
    }else{
		if( nread < PACKET_HEAD_LENGTH ){
			return true;
		}
		//这里读取的信息很可能包含多条信息，这时候需要解析出来；这几条信息因为太短，在发送时被底层socket合并了
		recvBufferPtr = recvBuffer;
        do{
            packetLength = *(int*)((void*)(recvBufferPtr));
			if( packetLength < PACKET_HEAD_LENGTH ){
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
				dispatchPacket(pPacket);
                pPacket->release();
                pPacket = NULL;
            }
            // 如果消息没有全部接收，那么将会放到临时包中等待下一次读数据操作
        }while(nread-(recvBufferPtr-recvBuffer) > PACKET_HEAD_LENGTH);
    }
	m_tempReadPacket = pPacket;
    return true;
}
bool Client::writePacket(void){
	Packet* pPacket = NULL;
	this->lock();
	if( !m_packetQueue.empty() ){
		pPacket = m_packetQueue.front();
		pPacket->retain();		// 引用 1
	}
	this->unlock();
	if( NULL == pPacket ){
		return true;
	}
	if( !writeSocket(pPacket) ){
		pPacket->release();		// 释放 1
		return false;
	}
	if( pPacket->isCursorEnd() ){
		pPacket->release();		// 释放 1
		// 反向检查队列并移除队末尾的对象
		bool releasePacketAtEnd = false;
		this->lock();
		if( !m_packetQueue.empty() && m_packetQueue.front() == pPacket ){
			releasePacketAtEnd = true;
			m_packetQueue.pop_front();
		}
		this->unlock();
		if(releasePacketAtEnd){
			pPacket->release();	// 对应进入队列时的retain
		}
		return true;
	}
	return true;
}
bool Client::writeSocket(Packet* pPacket){
    int nwrite;
    nwrite = write(m_fd, pPacket->getCursorPtr(), pPacket->getLength()-pPacket->getCursor());
    if(nwrite < 0){
        switch(errno){
        case EINTR: return true; // 写数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
//            fprintf(stderr, "write EAGAIN capture\n");
            return true;
        default: return false;
        }
        return false;
    }
    pPacket->moveCursor( nwrite );// used
    return true;
}
void Client::closeSocket(void){
	if( m_fd != 0 ){
        close( m_fd );
        m_fd = 0;
    }
}
bool Client::connectServer(void){
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *ptr = NULL;
	int fd = -1;
	int result;
	char port_str[8];
	sprintf(port_str, "%d", m_port);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	result = getaddrinfo(m_ip, port_str, &hints, &res);
	if( result ){
		freeaddrinfo(res);	// 记得释放
		return false;
	}
	for(ptr = res; ptr != NULL; ptr = ptr->ai_next){
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if( fd < 0 ){
			continue;
		}
		if( connect(fd, res->ai_addr, res->ai_addrlen) < 0 ){
			close(fd);
			fd = -1;
			continue;
		}
		break;	// we got one conn
	}
	freeaddrinfo(res);	// 记得释放
	if( fd < 0 ){
		return false;
	}
	m_fd = fd;
	return true;
}
//bool Client::connectServer(void){
//    int fd;
//    struct sockaddr_in servaddr;
//    socklen_t socklen = sizeof(struct sockaddr_in);
//    bzero(&servaddr, socklen);
//    servaddr.sin_family = AF_INET;
//    servaddr.sin_addr.s_addr = inet_addr( m_ip );//htonl(INADDR_ANY);
//    servaddr.sin_port = htons( m_port );
//
//    fd = socket(AF_INET, SOCK_STREAM, 0);
//    if(fd == -1){
//        return false;
//    }
//    if( connect(fd, (struct sockaddr *)&servaddr, socklen) == -1 ){
//        close( fd );
//        return false;
//    }
//    m_fd = fd;
//    return true;
//}
/*--------------------------------------------------------------------*/

NS_HIVE_END
