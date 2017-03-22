//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/15
// Time: 下午10:17
// To change this template use File | Settings | File Templates.
//

#include "multicurl.h"
#include "epollworker.h"
#include "globalservice.h"
#include "globalhandler.h"

NS_HIVE_BEGIN

static char curl_full_response[] = "Curl Queue Is Full!";
static char curl_timeout_response[] = "Curl Request Is Timeout!";

EasyCurl::EasyCurl(void) : EpollObject(), Object080816(), TimerObject(), RefObject(),
	m_pMultiCurl(NULL), m_pRequest(NULL), m_pResponse(NULL), m_pEasy(NULL), m_pHeader(NULL) {
	memset(m_pError, 0, CURL_ERROR_SIZE);
	setType(EASY_CURL_TYPE);
}
EasyCurl::~EasyCurl(void){
	getMultiCurl()->multiRemove(this);
	clearEasy();
	destroyEasy();
}
size_t EasyCurl::writeCallback(void *ptr, size_t size, size_t nmemb, void *data){
	EasyCurl* pEasyCurl = (EasyCurl*)data;
	size_t totalWrite = size*nmemb;
	pEasyCurl->appendResponse((char*)ptr, totalWrite);
	return totalWrite;
}
void EasyCurl::epollIn(void){
	getMultiCurl()->onCurlEpollIn(this);
}
void EasyCurl::epollOut(void){
	getMultiCurl()->onCurlEpollOut(this);
}
void EasyCurl::epollRemove(void){
	if(0 != m_fd){
		// 防止有数据没有删除
//		curl_multi_assign(getMultiCurl()->m_pMulti, m_fd, NULL);
		getEpoll()->objectRemove(this);
		m_fd = 0;
	}
}
int64 EasyCurl::timerCallback(void){
	LOG_DEBUG("remove easy curl");
	memcpy(this->m_pError, curl_timeout_response, sizeof(curl_timeout_response));
	getMultiCurl()->responseRequest(this, false);
	getMultiCurl()->removeAndClearEasy(this);
	return -1;
}
void EasyCurl::appendResponse(char* ptr, size_t length){
	Buffer* pResponse = this->createResponse(length);
	pResponse->write(ptr, (int)length, (int)pResponse->size());
}
void EasyCurl::clearEasy(void){
	epollRemove();
	releaseRequest();
	releaseResponse();
	releaseHeader();
	clearTimer();
}
bool EasyCurl::initializeEasy(RequestData* pRequest){
	if(NULL != m_pEasy){
		curl_easy_reset(m_pEasy);
	}else{
		m_pEasy = curl_easy_init();
	}
	if(NULL == m_pEasy){
		LOG_ERROR("easy is NULL");
		return false;
	}
	m_pError[0] = 0;
	curl_easy_setopt(m_pEasy, CURLOPT_WRITEFUNCTION, EasyCurl::writeCallback);
	curl_easy_setopt(m_pEasy, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(m_pEasy, CURLOPT_VERBOSE, 0L);		// 是否打印内部调试信息
	curl_easy_setopt(m_pEasy, CURLOPT_ERRORBUFFER, m_pError);
	curl_easy_setopt(m_pEasy, CURLOPT_PRIVATE, this);
	curl_easy_setopt(m_pEasy, CURLOPT_NOPROGRESS, 1L);	// 不需要进度信息
	curl_easy_setopt(m_pEasy, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(m_pEasy, CURLOPT_SSL_VERIFYPEER, 0L);	// https支持，不检测
	curl_easy_setopt(m_pEasy, CURLOPT_SSL_VERIFYHOST, 0L);	// https支持，不检测
	if(!this->setTimer(EASY_CURL_REQUEST_TIMEOUT, getMultiCurl()->getEpollWorker()->getTimer())){
		LOG_ERROR("easy is NULL");
		return false;
	}
	// 记录请求信息
	releaseRequest();
	m_pRequest = pRequest;
	m_pRequest->retain();
	// 设置请求的url
	curl_easy_setopt(m_pEasy, CURLOPT_URL, m_pRequest->m_url.c_str());
	// 设置Http请求信息Header
	releaseHeader();
	if( m_pRequest->m_headers.size() > 0 ){
		for(auto &header : m_pRequest->m_headers){
			m_pHeader = curl_slist_append(m_pHeader, header.c_str());
		}
		curl_easy_setopt(m_pEasy, CURLOPT_HTTPHEADER, m_pHeader);
	}
	// 设置post或者get相关的数据
	if( "POST" == m_pRequest->m_method ){
//		fprintf(stderr, "set post data to http request\n");
		curl_easy_setopt(m_pEasy, CURLOPT_POST, 1L);
		curl_easy_setopt(m_pEasy, CURLOPT_POSTFIELDS, m_pRequest->m_body->data());
		curl_easy_setopt(m_pEasy, CURLOPT_POSTFIELDSIZE, (long)m_pRequest->m_body->size());
	}
	return true;
}
void EasyCurl::destroyEasy(void){
	if(NULL != m_pEasy){
		curl_easy_cleanup(m_pEasy);
		m_pEasy = NULL;
	}
}


MultiCurl::MultiCurl(void) : RefObject(), TimerObject(),
	m_pMulti(NULL), m_pEasyCurlPool(NULL), m_pEpollWorker(NULL), m_pEpoll(NULL),
	m_timeoutCall(1), m_requestCount(0), m_stillRunning(0) {

}
MultiCurl::~MultiCurl(void){
	destroy();
}
int MultiCurl::socketCallback(CURL *pEasy, curl_socket_t s, int what, void *userp, void *socketp){
//	fprintf(stderr, "--MultiCurl::socketCallback time=%lld  s=%d what=%d\n", get_time_us(), s, what);
//	MultiCurl* pMultiCurl = (MultiCurl*)userp;
//	EasyCurl* pEasyCurl = (EasyCurl*)socketp;
	EasyCurl* pEasyCurl = NULL;
	if (what == CURL_POLL_REMOVE) {				// 4
//		if(NULL != pEasyCurl){
//			fprintf(stderr, "--MultiCurl::socketCallback curl set to idle time=%lld handle=%d\n", get_time_us(), pEasyCurl->getHandle());
//			curl_multi_assign(pMultiCurl->m_pMulti, s, NULL);
//		}
	}else{
		uint32 events = 0;
        if (what == CURL_POLL_IN) {				// 1
            events |= EPOLLIN;
        } else if (what == CURL_POLL_OUT) {		// 2
            events |= EPOLLOUT;
        } else if (what == CURL_POLL_INOUT) {	// 3
            events |= EPOLLIN | EPOLLOUT;
        }
        curl_easy_getinfo(pEasy, CURLINFO_PRIVATE, &pEasyCurl);
        if(pEasyCurl->getSocketFD() == 0){
//        if (NULL == pEasyCurl) {
//            curl_multi_assign(pMultiCurl->m_pMulti, s, pEasyCurl);
            pEasyCurl->setSocketFD(s);
			pEasyCurl->getEpoll()->objectAdd(pEasyCurl, events);
//			fprintf(stderr, "--MultiCurl::socketCallback s=%d first add to epoll\n", s);
		}else{
			pEasyCurl->getEpoll()->objectChange(pEasyCurl, events);
//			fprintf(stderr, "--MultiCurl::socketCallback s=%d change in epoll\n", s);
		}
	}
    return 0;
}
int MultiCurl::timeoutCallback(CURLM *pMulti, long timeout_ms, MultiCurl *pMultiCurl){
//	fprintf(stderr, "--MultiCurl::timeoutCallback time=%lld timeout_ms=%ld\n", get_time_us(), timeout_ms);

	if( timeout_ms < 0 ){
		pMultiCurl->clearTimer();
//		fprintf(stderr, "MultiCurl timer clear\n");
	}else{
		pMultiCurl->setTimer(timeout_ms, pMultiCurl->getEpollWorker()->getTimer());
//		fprintf(stderr, "MultiCurl timer add with timeout_ms=%ld\n", timeout_ms);
	}
	pMultiCurl->m_timeoutCall = 1;

	return 0;
}
int64 MultiCurl::timerCallback(void){
	curl_multi_socket_action(m_pMulti, CURL_SOCKET_TIMEOUT, 0, &m_stillRunning);
	if(m_stillRunning > 0){
		curl_multi_perform(m_pMulti, &m_stillRunning);
	}
	if( m_stillRunning < this->getRequestCount() ){
		this->multiInfoRead();
	}
	if(m_stillRunning > 0 && !this->isActive()){
		this->setTimer(m_timeoutCall, getEpollWorker()->getTimer());
		if(m_timeoutCall < MAX_PERFORM_CHECK_TIME){
			m_timeoutCall *= 2;
		}
	}
	return -1;
}
void MultiCurl::acceptRequestWithTask(RequestData* pRequest){
	CurlRequestTask* pTask = new CurlRequestTask();
	pTask->retain();
	pTask->setRequestData(pRequest);
	getEpollWorker()->acceptTask(pTask);
	pTask->release();
}
bool MultiCurl::acceptRequest(RequestData* pRequest){
	EasyCurl* pEasyCurl = this->createEasyCurl();
	if(NULL == pEasyCurl){
		if(queueRequest(pRequest)){
//			fprintf(stderr, "--MultiCurl::acceptRequest add to queue\n");
			return true;
		}
//		fprintf(stderr, "--MultiCurl::acceptRequest empty curl pool and full queue\n");
		return false;
	}
	if(!pEasyCurl->initializeEasy(pRequest)){
//		fprintf(stderr, "--MultiCurl::acceptRequest initializeEasy failed\n");
		goto CLEAR_EASY_CURL;
	}
	if(!this->multiAdd(pEasyCurl)){
//		fprintf(stderr, "--MultiCurl::acceptRequest multiAdd failed\n");
		goto CLEAR_EASY_CURL;
	}
	++m_requestCount;
//	fprintf(stderr, "--MultiCurl::acceptRequest OK\n");
	return true;
CLEAR_EASY_CURL:
	pEasyCurl->clearEasy();
	this->removeEasyCurl(pEasyCurl);
	return false;
}
void MultiCurl::multiInfoRead(void){
	CURLMsg *pMsg;
  	int msgsLeft;
	char *effUrl;
	EasyCurl *pEasyCurl;
	CURL *pEasy;
	CURLcode res;
	/* See how the transfers went */
	while((pMsg = curl_multi_info_read(m_pMulti, &msgsLeft))) {
		pEasy = pMsg->easy_handle;
		curl_easy_getinfo(pEasy, CURLINFO_PRIVATE, &pEasyCurl);
		if(pMsg->msg == CURLMSG_DONE) {
      		res = pMsg->data.result;
			if( res == CURLE_OK ){
				// 返回请求的结果
				this->responseRequest(pEasyCurl, true);
			}else{
				// 请求出错，打印错误结果
				curl_easy_getinfo(pEasy, CURLINFO_EFFECTIVE_URL, &effUrl);
				fprintf(stderr, "request url=%s result=%d falied: %s\n", effUrl, res, pEasyCurl->m_pError);
				// 返回请求错误
				this->responseRequest(pEasyCurl, false);
			}
//			fprintf(stderr, "--MultiCurl::multiInfoRead curl remove time=%lld handle=%d\n", get_time_us(), pEasyCurl->getHandle());
			this->removeAndClearEasy(pEasyCurl);
		}
	}
}
void MultiCurl::responseRequest(EasyCurl* pEasyCurl, bool isRequestOK){
	// 对方不需要返回结果
	if(pEasyCurl->m_pRequest->m_callbackID == 0){
		return;
	}
	uint32 handle = pEasyCurl->m_pRequest->m_originHandle;
	Buffer* pResponse = pEasyCurl->createResponse(CURL_ERROR_SIZE);
	if(!isRequestOK){
		pResponse->clear();
		pResponse->write(pEasyCurl->m_pError, CURL_ERROR_SIZE, 0);
	}
	CurlResponseTask* pTask = new CurlResponseTask();
	pTask->retain();
	pTask->setResponseData(pResponse);
	pTask->m_callbackID = pEasyCurl->m_pRequest->m_callbackID;
	pTask->m_isRequestOK = isRequestOK;
	GlobalService::getInstance()->dispatchTask(handle, pTask);
	pTask->release();
}
void MultiCurl::removeAndClearEasy(EasyCurl* pEasyCurl){
	// 从multi中移除这个对象
	this->multiRemove(pEasyCurl);
	--m_requestCount;
	// 清理对象数据
	pEasyCurl->clearEasy();

	RequestData* pRequest = this->getRequestQueue();
	while(NULL != pRequest){
		if(!pEasyCurl->initializeEasy(pRequest)){
//    		fprintf(stderr, "--MultiCurl::removeAndClearEasy initializeEasy failed\n");
    		pRequest->release();
    		pRequest = this->getRequestQueue();
    		continue;
    	}
    	if(!this->multiAdd(pEasyCurl)){
//    		fprintf(stderr, "--MultiCurl::removeAndClearEasy multiAdd failed\n");
    		pRequest->release();
    		pRequest = this->getRequestQueue();
			continue;
    	}
    	pEasyCurl->increaseVersion();	// 使用新的Handle
    	pRequest->release();
    	++m_requestCount;
//    	fprintf(stderr, "--MultiCurl::removeAndClearEasy add request from queue OK\n");
    	return;
	}

	// 释放到管理器中
	this->removeEasyCurl(pEasyCurl);
}
void MultiCurl::mcodeCheck(const char *where, CURLMcode code){
	if(CURLM_OK != code) {
		const char *s;
		switch (code) {
		case CURLM_BAD_HANDLE:
			s="CURLM_BAD_HANDLE";
			break;
		case CURLM_BAD_EASY_HANDLE:
			s="CURLM_BAD_EASY_HANDLE";
			break;
		case CURLM_OUT_OF_MEMORY:
			s="CURLM_OUT_OF_MEMORY";
			break;
		case CURLM_INTERNAL_ERROR:
			s="CURLM_INTERNAL_ERROR";
			break;
		case CURLM_UNKNOWN_OPTION:
			s="CURLM_UNKNOWN_OPTION";
			break;
		case CURLM_LAST:
			s="CURLM_LAST";
			break;
		default:
			s="CURLM_unknown";
			break;
		case CURLM_BAD_SOCKET:
			s="CURLM_BAD_SOCKET";
			LOG_ERROR("ERROR: %s returns %s", where, s);
			/* ignore this error */
			return;
		}
		LOG_ERROR("ERROR: %s returns %s", where, s);
		//exit(code);
	}
}
void MultiCurl::initialize(void){
	if(NULL == m_pEasyCurlPool){
		m_pEasyCurlPool = new EasyCurlPool();
		m_pEasyCurlPool->retain();
	}
	if(NULL != m_pMulti){
		return;
	}
	curl_global_init(CURL_GLOBAL_ALL);
	m_pMulti = curl_multi_init();
	if(NULL == m_pMulti){
//		fprintf(stderr, "--MultiCurl::initialize multi is NULL\n");
		return;
	}
	curl_multi_setopt(m_pMulti, CURLMOPT_SOCKETFUNCTION, MultiCurl::socketCallback);
	curl_multi_setopt(m_pMulti, CURLMOPT_SOCKETDATA, this);
	curl_multi_setopt(m_pMulti, CURLMOPT_TIMERFUNCTION, MultiCurl::timeoutCallback);
	curl_multi_setopt(m_pMulti, CURLMOPT_TIMERDATA, this);
//	fprintf(stderr, "--MultiCurl::initialize OK\n");
}
void MultiCurl::destroy(void){
	clearRequestQueue();
	SAFE_RELEASE(m_pEasyCurlPool)
	if(NULL != m_pMulti){
		curl_multi_cleanup(m_pMulti);
		m_pMulti = NULL;
	}
	curl_global_cleanup();
}

void CurlResponseTask::doTask(ActiveWorker* pHandler){
	EpollWorker* pWorker = (EpollWorker*)pHandler;
	pWorker->onCurlResponse(m_pResponse, m_callbackID, m_isRequestOK);
}

void CurlRequestTask::doTask(ActiveWorker* pHandler){
	EpollWorker* pWorker = (EpollWorker*)pHandler;
	if (!pWorker->getMultiCurl()->acceptRequest(m_pRequest)){
		LOG_ERROR("acceptRequest Failed");
		// callbackID 为0时，不需要返回调用结果
		if (m_pRequest->m_callbackID == 0){
			return;
		}
		// 返回请求错误的结果
		LOG_ERROR("curl queue is full handle=%d", m_pRequest->m_originHandle);
		Buffer* pResponse = m_pRequest->m_body;
		pResponse->clear();
		pResponse->write(curl_full_response, sizeof(curl_full_response), 0);
		CurlResponseTask* pTask = new CurlResponseTask();
		pTask->retain();
		pTask->setResponseData(pResponse);
		pTask->m_callbackID = m_pRequest->m_callbackID;
		pTask->m_isRequestOK = false;
		GlobalService::getInstance()->dispatchTask(m_pRequest->m_originHandle, pTask);
		pTask->release();
	}
}

NS_HIVE_END
