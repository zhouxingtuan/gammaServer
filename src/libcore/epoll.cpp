//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/30
// Time: 下午7:25
// To change this template use File | Settings | File Templates.
//

#include "epoll.h"

NS_HIVE_BEGIN

Epoll::Epoll() : RefObject(), m_pWorker(NULL), m_epollfd(0) {

}
Epoll::~Epoll(){
	closeEpoll();  // 关闭套接字
}

bool Epoll::createEpoll(void){
//    m_epollfd = epoll_create(MAX_WAIT_EVENT_SIZE);
    m_epollfd = epoll_create1(0);
	return true;
}
void Epoll::closeEpoll(void){
    if( m_epollfd != 0 ){
        close(m_epollfd);
        m_epollfd = 0;
    }
}

NS_HIVE_END
