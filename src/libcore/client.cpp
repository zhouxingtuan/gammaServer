//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/31
// Time: 下午9:41
// To change this template use File | Settings | File Templates.
//

#include "client.h"
#include "epoll.h"
#include "globalservice.h"
#include "epollworker.h"
#include "mainworker.h"

NS_HIVE_BEGIN

Client::Client(void) : Accept() {

}
Client::~Client(void){

}
bool Client::connectServer(void){
    int fd;
    struct sockaddr_in servaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    bzero(&servaddr, socklen);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr( this->getIP() );//htonl(INADDR_ANY);
    servaddr.sin_port = htons( this->getPort() );

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        return false;
    }
    if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK) == -1){
  		return false;
    }
    if( connect(fd, (struct sockaddr *)&servaddr, socklen) == -1 ){
    	if(errno != EINPROGRESS){
			close( fd );
			return false;
    	}
    }
    this->setSocketFD(fd);
    return true;
}
bool Client::epollActive(uint32 events){
	if( getConnectionState() < CS_CONNECT_OK ){
		int error;
		socklen_t len = sizeof(error);
		if( getsockopt(this->getSocketFD(), SOL_SOCKET, SO_ERROR, &error, &len) < 0 ){
			fprintf(stderr, "--Client::epollActive 1 failed to connect to handle=%d ip=%s port=%d\n", getHandle(), getIP(), getPort());
			closeSocket();
			getEpollWorker()->notifyCloseConnect(this);
			getEpollWorker()->closeClient(this->getHandle());
		}
		if(error){
			if(error == EINTR || error == EINPROGRESS){
				fprintf(stderr, "--Client::epollActive connect ing\n");
				return true;
			}
			fprintf(stderr, "--Client::epollActive 2 failed to connect to handle=%d ip=%s port=%d\n", getHandle(), getIP(), getPort());
			closeSocket();
			getEpollWorker()->notifyCloseConnect(this);
			getEpollWorker()->closeClient(this->getHandle());
			return true;
		}
		fprintf(stderr, "--Client::epollActive connect OK to handle=%d ip=%s port=%d\n", getHandle(), getIP(), getPort());
		getEpollWorker()->receiveClient(this);
		return true;
	}
//	fprintf(stderr, "--Client::epollActive OK getConnectionState()=%d\n", getConnectionState());
	return false;
}
void Client::epollRemove(void){
	getEpollWorker()->notifyCloseConnect(this);
	getEpollWorker()->closeClient(this->getHandle());
}

NS_HIVE_END
