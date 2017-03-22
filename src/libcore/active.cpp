//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/22
// Time: 下午10:00
// To change this template use File | Settings | File Templates.
//

#include "active.h"

NS_HIVE_BEGIN

Active::Active(void) : EpollObject() {

}
Active::~Active(void){
	closeEvent();
}

bool Active::epollActive(uint32 events){
	this->readEvent();
	getEpoll()->objectChange(this, EPOLLIN);
	return true;
}
int Active::readEvent(void){
    uint64 value;
    int nread;
    nread = read(m_fd, &value, sizeof(uint64));
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
	// do nothing for the event
//	fprintf(stderr, "--Active::readEvent recv event = %llu\n", value);
	this->onActive(value);
	return 0;
}
void Active::closeEvent(void){
	if(0 != m_fd){
		getEpoll()->objectRemove(this);
		close(m_fd);
		m_fd = 0;
	}
}
bool Active::createEvent(Epoll* pEpoll){
	m_fd = eventfd(0, 0);
	if (m_fd == -1){
		LOG_ERROR("m_fd == -1");
		goto ACTIVE_CREATE_FAILED;
	}
	if (!set_non_blocking(m_fd)){
		LOG_ERROR("set_non_blocking m_fd failed");
		goto ACTIVE_CREATE_FAILED;
	}
	if (!pEpoll->objectAdd(this, EPOLLIN)){
		LOG_ERROR("objectAdd m_fd failed");
		goto ACTIVE_CREATE_FAILED;
	}
	return true;
ACTIVE_CREATE_FAILED:
	closeEvent();
	return false;
}

NS_HIVE_END
