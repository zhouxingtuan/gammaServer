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

NS_HIVE_BEGIN

Accept::Accept(void) : EpollConnectObject(), Destination(), TimerObject(),
 	m_timerCallback(NULL), m_pEpollWorker(NULL), m_tempReadPacket(NULL), m_bindHandle(0),
 	m_connectionState(CS_DISCONNECT), m_isOnline(0),
	m_isNeedEncrypt(false), m_isNeedDecrypt(false) {

}
Accept::~Accept(void){
	releasePacket();
	SAFE_RELEASE(m_tempReadPacket)
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
void Accept::dispatchPacket(Packet* pPacket){
	// 对收到的消息进行解密处理：从body开始解密；头部已经在判断长度的时候解密
	if( this->isNeedDecrypt() ){
		binary_decrypt(pPacket->getDataPtr()+8, pPacket->getLength()-8, MainWorker::getInstance()->getKey());
	}
	GlobalService::getInstance()->dispatchToService(pPacket);
}
int Accept::readSocket(void){
//	char recvBuffer[8192];
	char* recvBuffer = getEpollWorker()->getReadBuffer();
    char* recvBufferPtr;
    int nread;
    int packetLength;
    int writeLength;
    Packet* pPacket;
    pPacket = m_tempReadPacket;
    if( pPacket == NULL ){
        nread = read(this->getSocketFD(), recvBuffer, EPOLL_READ_BUFFER_SIZE);
    }else{
        nread = read(this->getSocketFD(), pPacket->getCursorPtr(), pPacket->getLength()-pPacket->getCursor());
    }
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
		if( nread < (int)sizeof(PacketHead) ){
			return 0;
		}
		//这里读取的信息很可能包含多条信息，这时候需要解析出来；这几条信息因为太短，在发送时被底层socket合并了
		recvBufferPtr = recvBuffer;
        do{
        	// 对头部数据进行解密
        	if( this->isNeedDecrypt() ){
        		binary_decrypt(recvBufferPtr, 8, MainWorker::getInstance()->getKey());
        	}
            packetLength = *(int*)((void*)(recvBufferPtr));
			if( packetLength < (int)sizeof(PacketHead) || packetLength > getMaxLength() ){
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
				dispatchPacket(pPacket);
                pPacket->release();
                pPacket = NULL;
            }
            // 如果消息没有全部接收，那么将会放到临时包中等待下一次读数据操作
        }while(nread-(recvBufferPtr-recvBuffer) > (int)sizeof(PacketHead));
    }
	m_tempReadPacket = pPacket;
    return 0;
}
int Accept::writeSocket(Packet* pPacket){
	// 检查是否已经经过加密操作
	if( pPacket->getBuffer()->checkEncryptFlag() ){
		// 发送数据前，记录一次数据总长度；确保不会出错
		pPacket->recordLength();
		if( this->isNeedEncrypt() ){
			binary_encrypt(pPacket->getDataPtr(), pPacket->getLength(), MainWorker::getInstance()->getKey());
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

NS_HIVE_END
