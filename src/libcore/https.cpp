//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/1/23
// Time: 下午6:27
// To change this template use File | Settings | File Templates.
//

#include "https.h"
#include "globalservice.h"
#include "epollworker.h"

NS_HIVE_BEGIN

Https::Https(void) : Http(), m_pSSL(NULL) {

}
Https::~Https(void){
	destroySSL();
}

int Https::readSocket(void){
//	char recvBuffer[8192];
	char* recvBuffer = getEpollWorker()->getReadBuffer();
	int nread;
//	nread = read(this->getSocketFD(), recvBuffer, 8192);
	nread = SSL_read(m_pSSL, recvBuffer, EPOLL_READ_BUFFER_SIZE);
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
	Buffer* pBuffer = createBuffer(nread);
	this->setOffset((int)pBuffer->size());
	pBuffer->insert(pBuffer->end(), recvBuffer, recvBuffer + nread);
	return 0;
}
int Https::writeSocket(void){
    int nwrite;
//    nwrite = write(this->getSocketFD(), this->getWriteOffsetPtr(), this->getWriteSize());
    nwrite = SSL_write(m_pSSL, this->getWriteOffsetPtr(), this->getWriteSize());
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
    moveWriteOffset(nwrite);
    if( this->isWriteEnd() ){
    	return 0;
    }
    return 3;	// 一次没能写完，等待写
}
bool Https::bindSSL(SSL_CTX* ctx){
	/* 基于 ctx 产生一个新的 SSL*/
    m_pSSL = SSL_new(ctx);
    /* 将连接用户的 socket 加入到 SSL*/
    SSL_set_fd(m_pSSL, this->getSocketFD());
    /* 建立 SSL 连接 */
    if (SSL_accept(m_pSSL) == -1) {
		LOG_ERROR("SSL_accept failed");
    	return false;
    }
    return true;
}
void Https::resetData(void){
	destroySSL();
	Http::resetData();
}
void Https::destroySSL(void){
	if(NULL != m_pSSL){
		/* 关闭 SSL 连接 */
		SSL_shutdown(m_pSSL);
        /* 释放 SSL*/
        SSL_free(m_pSSL);
        m_pSSL = NULL;
	}
}

NS_HIVE_END
