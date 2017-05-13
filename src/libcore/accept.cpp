//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/31
// Time: 下午9:40
// To change this template use File | Settings | File Templates.
//

#include "accept.h"
#include "epoll.h"
#include "globalservice.h"
#include "epollworker.h"
#include "mainworker.h"
#include "dispatcher.h"
#include "globalsetting.h"

NS_HIVE_BEGIN

Accept::Accept(void) : EpollConnectObject(), TimerObject(),
 	m_timerCallback(NULL), m_pEpollWorker(NULL), m_tempReadPacket(NULL), m_bindHandle(0),
 	m_connectionState(CS_DISCONNECT), m_isOnline(0),
	m_isNeedEncrypt(false), m_isNeedDecrypt(false), m_acceptIndex(0) {

}
Accept::~Accept(void){
	releasePacket();
	SAFE_RELEASE(m_tempReadPacket)
}
bool Accept::epollActive(uint32 events){
	return false;
}
void Accept::epollIn(void){
	int result;
	do{
		result = readSocket();
	}while(result == 0);
	if( result < 0 ){
		epollRemove();
	}
}
void Accept::epollOut(void){
	Packet* pPacket;
	int result;
	do{
		pPacket = NULL;
		if( !m_packetQueue.empty() ){
			pPacket = m_packetQueue.front();
			m_packetQueue.pop_front();
		}
		if( NULL == pPacket ){
			//getEpoll()->objectChange(this, EPOLLIN);
			return;
		}
		result = writeSocket(pPacket);
		if( result < 0 ){
			pPacket->release();		// 释放
			epollRemove();
			return;
		}
		// result == 0 成功写 || result > 0 需要重新尝试写
		if(pPacket->isCursorEnd()){
			pPacket->release();		// 释放
		}else{
			// 没写完就说明写入buffer已经满了，等待下一次写操作
			m_packetQueue.push_front(pPacket);
			return;
		}
	}while(1);
}
void Accept::epollRemove(void){
	getEpollWorker()->notifyCloseConnect(this);
	getEpollWorker()->closeAccept(this->getHandle());
}
void Accept::epollCheck(void){
	if( m_packetQueue.empty() ){
		getEpoll()->objectChange(this, EPOLLIN);
	}else{
		getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
	}
}
void Accept::onReceivePacket(Packet* pPacket, Task* pTask){
	AcceptReceivePacketFunction func = GlobalSetting::getInstance()->getAcceptReceivePacketFunction(this->getAcceptIndex());
    if(NULL == func){
		LOG_ERROR("can not find accept receive packet function for index = %d", this->getAcceptIndex());
    }else{
    	func(this, pPacket);
    }
}
int64 Accept::timerCallback(void){
	if(NULL != m_timerCallback){
		return m_timerCallback(this);
	}
	return -1;
}
bool Accept::setTimeout(int64 timeCount, Timer* pTimer, ConnectTimeoutCallback callback){
	m_timerCallback = callback;
	return setTimer(timeCount, pTimer);
}

void Accept::releasePacket(void){
	for( auto pPacket : m_packetQueue ){
		pPacket->release();
	}
	m_packetQueue.clear();
}
bool Accept::sendPacket(Packet* pPacket){
	pPacket->resetCursor();		// 后面的写操作需要重置
	pPacket->retain();			// 进入队列前引用
	if( !m_packetQueue.empty() ){
		// 已经在epoll中等待out事件
		m_packetQueue.push_back(pPacket);
	}else{
		// 先执行写操作，如果出现重试，那么再将Packet入队
		int result = writeSocket(pPacket);
		if( result < 0 ){
			pPacket->release();		// 释放
			epollRemove();
		}else{
			// result == 0 成功写 || result > 0 需要重新尝试写
			if(pPacket->isCursorEnd()){
				pPacket->release();		// 释放
			}else{
				// 没有写完的消息进入队列
				m_packetQueue.push_front(pPacket);
				// 进入epoll等待
				getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
			}
		}
	}

	return true;
}
bool Accept::sendPacket(const char* ptr, int length){
	if( !m_packetQueue.empty() ){
		Packet* pPacket = new Packet(length);
		pPacket->write(ptr, length);
		pPacket->resetCursor();		// 后面的写操作需要重置
		pPacket->retain();			// 进入队列前引用
		// 已经在epoll中等待out事件
		m_packetQueue.push_back(pPacket);
	}else{
		// 先执行写操作，如果出现重试，那么再将Packet入队
		int writeLength = 0;
		int result = writeSocket(ptr, length, &writeLength);
		if( result < 0 ){
			epollRemove();
		}else{
			// result == 0 成功写 || result > 0 需要重新尝试写
			if(length == writeLength){
				// write success end, do nothing
			}else{
				int leftLength;
				if(result == 0){
					leftLength = length - writeLength;
				}else{
					leftLength = length;
				}
				Packet* pPacket = new Packet(leftLength);
				pPacket->write(ptr + writeLength, leftLength);
				pPacket->resetCursor();		// 后面的写操作需要重置
				pPacket->retain();			// 进入队列前引用
				// 没有写完的消息进入队列
				m_packetQueue.push_front(pPacket);
				// 进入epoll等待
				getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
			}
		}
	}

	return true;
}
void Accept::resetData(void){
	setConnectionState(CS_DISCONNECT);
//	setPingTime(0);
	SAFE_RELEASE(m_tempReadPacket)
	clearTimer();		// 停止计时器
	closeSocket();		// 关闭套接字
	releasePacket();	// 取消所有数据包的发送
	m_bindHandle = 0;
	m_isNeedEncrypt = false;
	m_isNeedDecrypt = false;
}
void Accept::dispatchPacket(Packet* pPacket, uint8 command){
	LOG_DEBUG("Accept handle=%d dispatchPacket command=%d", getHandle(), command);
	// 对收到的消息进行解密处理：从body开始解密；头部已经在判断长度的时候解密
	if( this->isNeedDecrypt() ){
		LOG_DEBUG("Accept handle=%d dispatchPacket command=%d need decrypt", getHandle(), command);
		GlobalSetting::getInstance()->getAcceptDecryptFunction()(this, pPacket);
	}
	// 判断当前的连接是否已经验证
	if(getConnectionState() < CS_IDENTIFY_OK && getType() == POOL_TYPE_ACCEPT && command != COMMAND_REGISTER){
		LOG_ERROR("Accept is not identify yet. close the connection handle=%d command=%d", getHandle(), command);
		epollRemove();
		return;
	}
	// 判断cmd执行后续操作
	AcceptCommandFunction func = GlobalSetting::getInstance()->getAcceptCommandFunction(command);
	if(NULL == func){
		LOG_DEBUG("Accept handle=%d dispatchPacket command=%d function not found.", getHandle(), command);
		// 这里不执行的命令，发送消息给后面的服务执行
		pPacket->setDestination(this->getHandle());
		Dispatcher::getInstance()->dispatchCommand(pPacket, command);
	}else{
		func(this, pPacket, command);
	}
}
int Accept::readSocket(void){
	char* recvBuffer = getEpollWorker()->getReadBuffer();
	int nread;
	nread = read(this->getSocketFD(), recvBuffer, EPOLL_READ_BUFFER_SIZE);
    if(nread < 0){
        switch(errno){
        case EINTR: return 1; 	// 读数据失败，处理信号中断
        case EAGAIN: return 2;	// 可以下次重新调用
        default: return -1;
        }
        return -1;
    }else if(nread == 0){
        return -1;
    }
    AcceptReadFunction func = GlobalSetting::getInstance()->getAcceptReadFunction(this->getAcceptIndex());
    if(NULL == func){
		LOG_ERROR("can not find accept read function for index = %d", this->getAcceptIndex());
    }else{
    	func(this, recvBuffer, nread);
    }
    return 0;
}
int Accept::writeSocket(Packet* pPacket){
	// 检查是否已经经过加密操作
	if( pPacket->getBuffer()->checkEncryptFlag() ){
		if( this->isNeedEncrypt() ){
			GlobalSetting::getInstance()->getAcceptEncryptFunction()(this, pPacket);
		}
	}
    int nwrite;
    nwrite = write(this->getSocketFD(), pPacket->getCursorPtr(), pPacket->getLength()-pPacket->getCursor());
    if(nwrite < 0){
        switch(errno){
        case EINTR: return 1; // 写数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
//            fprintf(stderr, "write EAGAIN capture\n");
            return 2;
        default: return -1;
        }
        return -1;
    }
    pPacket->moveCursor( nwrite );// used
    return 0;
}
int Accept::writeSocket(const char* ptr, int length, int* writeLength){

    int nwrite;
    nwrite = write(this->getSocketFD(), ptr, length);
    if(nwrite < 0){
        switch(errno){
        case EINTR: return 1; // 写数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
//            fprintf(stderr, "write EAGAIN capture\n");
            return 2;
        default: return -1;
        }
        return -1;
    }
    *writeLength = nwrite;
    return 0;
}

NS_HIVE_END
