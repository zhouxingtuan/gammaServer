//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/2
// Time: 下午11:26
// To change this template use File | Settings | File Templates.
//

#include "http.h"
#include "globalservice.h"
#include "epollworker.h"
#include "mainworker.h"

NS_HIVE_BEGIN

Http::Http(void) : EpollConnectObject(), TimerObject(), m_pEpollWorker(NULL), m_pBuffer(NULL),
	m_httpState(HTTP_STATE_IDLE), m_parseState(HTTP_PARSE_INIT), m_isInEpoll(false), m_offset(0){

}
Http::~Http(void){
	this->resetData();
}

int Http::onMessageBegin(http_parser* pParser){
	((Http*)pParser->data)->setParseState(HTTP_PARSE_MESSAGE_BEGIN);
	return 0;
}
int Http::onHeadersComplete(http_parser* pParser){
	((Http*)pParser->data)->setParseState(HTTP_PARSE_HEADER_COMPLETE);
	return 0;
}
int Http::onMessageComplete(http_parser* pParser){
	((Http*)pParser->data)->setParseState(HTTP_PARSE_MESSAGE_END);
	return 0;
}
int Http::onUrl(http_parser* pParser, const char* at, size_t length){
	return ((Http*)pParser->data)->recordUrl(at, length);
}
int Http::onHeaderField(http_parser* pParser, const char* at, size_t length){
	return ((Http*)pParser->data)->recordHeaderField(at, length);
}
int Http::onHeaderValue(http_parser* pParser, const char* at, size_t length){
	return ((Http*)pParser->data)->recordHeaderValue(at, length);
}
int Http::onBody(http_parser* pParser, const char* at, size_t length){
	return ((Http*)pParser->data)->recordBody(at, length);
}
int Http::recordUrl(const char* at, size_t length){
	if(HTTP_PARSE_URL != getParseState()){
		m_url.offset = this->getOffsetAt(at);
		m_url.length = (int)length;
		this->setParseState(HTTP_PARSE_URL);
	}else{
		m_url.length += (int)length;
	}
	return 0;
}
int Http::recordHeaderField(const char* at, size_t length){
//	fprintf(stderr, "Http::recordHeaderField offset=%d length=%d\n", this->getOffsetAt(at), length);
	if(HTTP_PARSE_HEADER_FIELD != getParseState()){
		HttpHeaderRecorder header;
		header.field.offset = this->getOffsetAt(at);
		header.field.length = (int)length;
		m_headers.push_back(header);
		this->setParseState(HTTP_PARSE_HEADER_FIELD);
	}else{
		HttpHeaderRecorder& header = m_headers.back();
		header.field.length += (int)length;
	}
	return 0;
}
int Http::recordHeaderValue(const char* at, size_t length){
//	fprintf(stderr, "Http::recordHeaderValue offset=%d length=%d\n", this->getOffsetAt(at), length);
	if(HTTP_PARSE_HEADER_VALUE != getParseState()){
		HttpHeaderRecorder& header = m_headers.back();
		header.value.offset = this->getOffsetAt(at);
		header.value.length = (int)length;
		this->setParseState(HTTP_PARSE_HEADER_VALUE);
	}else{
		HttpHeaderRecorder& header = m_headers.back();
		header.value.length += (int)length;
	}
	return 0;
}
int Http::recordBody(const char* at, size_t length){
//	fprintf(stderr, "Http::recordBody offset=%d length=%d\n", this->getOffsetAt(at), length);
	if(HTTP_PARSE_BODY != getParseState()){
		m_body.offset = this->getOffsetAt(at);
		m_body.length = (int)length;
		this->setParseState(HTTP_PARSE_BODY);
	}else{
		m_body.length += (int)length;
	}
	return 0;
}
bool Http::epollActive(uint32 events){
	// 检查是读事件还是写事件；根据当前状态执行下一个处理动作
	if( this->getState() >= HTTP_STATE_WRITE_DONE ){
		this->epollRemove();	// main主线程，在这里检查删除
		return true;
	}
	if(events & EPOLLIN){
		this->setState(HTTP_STATE_READ);
		this->epollIn();
	}
	if(events & EPOLLOUT){
		this->setState(HTTP_STATE_WRITE);
		this->epollOut();
	}else if(events & EPOLLERR){
		this->epollRemove();	// main主线程，在这里检查删除
		return true;
	}
	return true;
}
void Http::epollIn(void){
	int result;
	size_t nparsed;
	int parse_size;
	do{
		result = readSocket();
		if(NULL == this->getBuffer()){
			LOG_ERROR("buffer not create readSocket result=%d", result);
			break;
		}
		parse_size = this->getParseSize();
		nparsed = http_parser_execute(this->getParser(), getEpollWorker()->getSettings(), this->getParseOffsetPtr(), parse_size);
		if (this->getParser()->upgrade) {
			result = -1;		// 不支持WebSocket
			break;
		}else if(nparsed != (size_t)parse_size){
			result = -1;
			break;
		}
		// 一旦解析结束，调用处理请求的callback
		if(HTTP_PARSE_MESSAGE_END == getParseState()){
			this->setState(HTTP_STATE_READ_DONE);
			this->setOffset(0);
			// 回调处理收到请求
//			fprintf(stderr, "%s\n", m_pBuffer->data());
			GlobalSetting::getInstance()->getReceiveHttpFunction()(this);
			result = 0;
			break;
		}
	}while(result == 0);
	if(result < 0){
		this->setState(HTTP_STATE_DESTROY);
	}else if(result > 0){
		checkEpollState(EPOLLIN);
		this->setState(HTTP_STATE_READ_WAIT);
	}else{
		// do nothing
	}
    // 不管成功还是失败，都会检查删除
	if( this->getState() >= HTTP_STATE_WRITE_DONE ){
		this->epollRemove();
	}
}
void Http::epollOut(void){
	int result;
	do{
		result = writeSocket();
		if( result < 0 ){
			this->setState(HTTP_STATE_DESTROY);
			break;
		}
		if( result == 0 ){
			this->setState(HTTP_STATE_WRITE_DONE);
			break;
		}
		// result > 0
		checkEpollState(EPOLLOUT);
		this->setState(HTTP_STATE_WRITE_WAIT);
		break;
	}while(0);
    // 不管成功还是失败，都会检查删除
	if( this->getState() >= HTTP_STATE_WRITE_DONE ){
		this->epollRemove();
	}
}
void Http::epollRemove(void){
	LOG_DEBUG("handle=%d", this->getHandle());
	// 移除回调
	GlobalSetting::getInstance()->getRemoveHttpFunction()(this);
	// 清理状态，移除出epoll
	this->resetData();
	getEpollWorker()->closeHttp(this->getHandle());
}
bool Http::checkEpollState(uint32 events){
	if( this->isInEpoll() ){
		getEpoll()->objectChange(this, events);
	}else{
		if( !getEpoll()->objectAdd(this, events) ){
			this->setState(HTTP_STATE_DESTROY);
		}else{
			setIsInEpoll(true);
		}
	}
	return true;
}
void Http::checkEpollRemove(void){
	if( this->isInEpoll() ){
		getEpoll()->objectRemove(this);
		this->setIsInEpoll(false);
	}
}
void Http::onReceivePacket(Packet* pPacket, Task* pTask){
	GlobalSetting::getInstance()->getHttpReceivePacketFunction()(this, pPacket);
}
int64 Http::timerCallback(void){
	this->epollRemove();		// main主线程，在这里检查删除
	return -1;
}
void Http::responseRequest(const char* ptr, uint32 length){
	Buffer* pBuffer = this->createBuffer(length);
	pBuffer->clear();
	pBuffer->write(ptr, (int)length, 0);
	this->setOffset(0);
	this->epollOut();
}
int Http::readSocket(void){
//	char recvBuffer[8192];
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
	Buffer* pBuffer = createBuffer(nread);
	this->setOffset((int)pBuffer->size());
	pBuffer->insert(pBuffer->end(), recvBuffer, recvBuffer + nread);
	return 0;
}
int Http::writeSocket(void){
    int nwrite;
    nwrite = write(this->getSocketFD(), this->getWriteOffsetPtr(), this->getWriteSize());
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
void Http::resetData(void){
	this->checkEpollRemove();
	this->closeSocket();
	this->clearState();
	this->clearTimer();
	this->releaseBuffer();
	this->setParseState(HTTP_PARSE_INIT);
	this->setOffset(0);
	this->m_url.reset();
	this->m_body.reset();
	this->m_headers.clear();
}
void Http::initialize(void){
	http_parser* pParser = getParser();
	http_parser_init(pParser, HTTP_REQUEST);
	pParser->data = this;
	this->setParseState(HTTP_PARSE_INIT);
}



NS_HIVE_END
