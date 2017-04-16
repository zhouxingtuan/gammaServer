//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/30
// Time: 下午7:25
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__epoll__
#define __hive__epoll__

#include "common.h"
#include "destination.h"

NS_HIVE_BEGIN

#define MAX_WAIT_EVENT_SIZE 10240
#define EPOLL_WAIT_FLAG -1

#define IP_SIZE 18
typedef struct SocketInformation{
    int fd;
    char ip[IP_SIZE];//192.168.110.110
    uint16 port;
}SocketInformation;

class Epoll;
class ActiveWorker;

class EpollObject : public Destination
{
protected:
	int m_fd;
	Epoll* m_pEpoll;
public:
	EpollObject(void) : Destination(), m_fd(0), m_pEpoll(NULL) {

	}
	virtual ~EpollObject(void){}

	// from Destination
	virtual void onReceivePacket(Packet* pPacket, Task* pTask){}

	// 对象被epoll激活时调用的函数，用于Listener
	virtual bool epollActive(uint32 events) = 0;
	// 处理数据读
	virtual void epollIn(void) = 0;
	// 处理数据写
	virtual void epollOut(void) = 0;
	// 处理移除socket
	virtual void epollRemove(void) = 0;
	// 处理socket最后的状态
	virtual void epollCheck(void) = 0;

	inline void closeSocket(void){
		if( m_fd != 0 ){
			close( m_fd );
			m_fd = 0;
		}
	}
	inline void setEpoll(Epoll* pEpoll){ m_pEpoll = pEpoll; }
	inline Epoll* getEpoll(void){ return m_pEpoll; }
	inline int getSocketFD(void) const { return m_fd; }
	inline void setSocketFD(int fd){ m_fd = fd; }
};

class EpollConnectObject : public EpollObject
{
protected:
    char m_ip[IP_SIZE];//192.168.110.110
    uint16 m_port;
public:
	EpollConnectObject(void) : EpollObject(), m_port(0){
		memset(m_ip, 0, IP_SIZE);
	}
	virtual ~EpollConnectObject(void){ closeSocket(); }

	inline void setSocket(const char* ip, uint16 port){
		strcpy(m_ip, ip);
		m_port = port;
	}
	inline const char* getIP(void) const { return m_ip; }
	inline const uint16 getPort(void) const { return m_port; }
};

inline bool set_non_blocking(int fd){
    if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK) == -1){
  		return false;
    }
    return true;
}

class Epoll : public RefObject
{
protected:
	ActiveWorker* m_pWorker;
	int m_epollfd;
	struct epoll_event m_events[MAX_WAIT_EVENT_SIZE];
public:
	Epoll(void);
	virtual ~Epoll(void);

	void setWorker(ActiveWorker* pWorker) { m_pWorker = pWorker; }
	ActiveWorker* getWorker(void) { return m_pWorker; }

	// EPOLLIN | EPOLLOUT | EPOLLET
	inline bool objectAdd(EpollObject* pObject, uint32 events=EPOLLIN){
		int fd = pObject->getSocketFD();
		if(0 == fd){
			return false;
		}
    	struct epoll_event ev;
    	ev.events = events | EPOLLET;
        ev.data.ptr = pObject;
        if( epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1){
        	return false;
        }
        pObject->setEpoll(this);
        return true;
	}
	inline void objectChange(EpollObject* pObject, uint32 events){
		int fd = pObject->getSocketFD();
		if(fd == 0){
			return;
		}
		struct epoll_event ev;
		ev.events = events | EPOLLET;
		ev.data.ptr = pObject;
		epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &ev);
	}
	inline void objectRemove(EpollObject* pObject){
		int fd = pObject->getSocketFD();
		if( 0 != fd ){
    		epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL);
    	}
	}
	inline bool update(int timeout){
		EpollObject* pObject;
		uint32 events;
		int nfds;
		nfds = epoll_wait(m_epollfd, m_events, MAX_WAIT_EVENT_SIZE, timeout);
		if(nfds > 0){
			for(int i=0; i<nfds; ++i){
				struct epoll_event& evt = m_events[i];
				pObject = (EpollObject*)evt.data.ptr;
				events = evt.events;
				if( pObject->epollActive(events) ){
					continue;
				}
				if(events & EPOLLIN){
					pObject->epollIn();
				}
				if(events & EPOLLOUT){
					pObject->epollOut();
				}else if(events & EPOLLERR){
					pObject->epollRemove();
					continue;
				}
				pObject->epollCheck();
			}
		}
		return true;
	}
	bool createEpoll(void);
	void closeEpoll(void);
};

NS_HIVE_END

#endif
