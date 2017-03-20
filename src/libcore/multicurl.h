//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/15
// Time: 下午10:17
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__multicurl__
#define __hive__multicurl__

#include "curl/curl.h"
#include "epoll.h"
#include "object.h"
#include "packet.h"
#include "task.h"
#include "active.h"
#include "util.h"
#include "timer.h"
#include "handler.h"

NS_HIVE_BEGIN

#define EASY_CURL_REQUEST_TIMEOUT 10000
#define EASY_CURL_TYPE 5
#define MAX_REQUEST_PERFORM 1024
#define MAX_REQUEST_QUEUE 100000
#define MAX_PERFORM_CHECK_TIME 128

class MultiCurl;
class EpollWorker;

class RequestData : public RefObject
{
public:
	typedef std::vector<std::string> StringVector;
	RequestData(void) : RefObject(), m_body(NULL), m_originHandle(0), m_callbackID(0){}
	RequestData(const char* method, const char* url, const char* body, uint32 body_length, uint32 handle, uint32 callbackID)
		: RefObject(), m_method(method), m_url(url), m_body(NULL), m_originHandle(handle), m_callbackID(callbackID){
		m_body = new Buffer(body_length);
		m_body->retain();
		m_body->write(body, body_length, 0);
	}
	virtual ~RequestData(void){
		SAFE_RELEASE(m_body)
	}

	void appendHeader(const char* field, const char* value){
		std::string header;
		header.append(field);
		header.append(": ");
		header.append(value);
		m_headers.push_back(header);
	}
public:
	std::string m_method;
	std::string m_url;
	StringVector m_headers;
	Buffer* m_body;
	uint32 m_originHandle;
	uint32 m_callbackID;
};

class EasyCurl : public EpollObject, public Object080816, public TimerObject, public RefObject
{
public:
	friend class MultiCurl;
public:
	EasyCurl(void);
	virtual ~EasyCurl(void);

	static EasyCurl* createObject(void){
		EasyCurl* pEasyCurl = new EasyCurl();
		pEasyCurl->retain();
		return pEasyCurl;
	}
	static void releaseObject(EasyCurl* pEasyCurl){
		if(NULL != pEasyCurl){
			pEasyCurl->release();
		}
	}
	static size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *data);

	// from EpollObject
	virtual bool epollActive(uint32 events){ return false; }
	virtual void epollIn(void);
	virtual void epollOut(void);
	virtual void epollRemove(void);
	virtual void epollCheck(void){}

	// from TimerObject
	virtual int64 timerCallback(void);

	inline bool isOK(void){ return (NULL != m_pEasy); }
	bool initializeEasy(RequestData* pRequest);
	void clearEasy(void);
protected:
	inline void releaseResponse(void){
		SAFE_RELEASE(m_pResponse)
	}
	inline void releaseRequest(void){
		SAFE_RELEASE(m_pRequest)
	}
	inline void releaseHeader(void){
		if(NULL != m_pHeader){
			curl_slist_free_all(m_pHeader);
			m_pHeader = NULL;
		}
	}
	inline void setMultiCurl(MultiCurl* pMultiCurl){ m_pMultiCurl = pMultiCurl; }
	inline MultiCurl* getMultiCurl(void){ return m_pMultiCurl; }
	void destroyEasy(void);
	inline Buffer* createResponse(uint32 length){
		if(NULL == m_pResponse){
			m_pResponse = new Buffer(length);
			m_pResponse->retain();
		}
		return m_pResponse;
	}
	void appendResponse(char* ptr, size_t length);
protected:
	MultiCurl* m_pMultiCurl;
	RequestData* m_pRequest;		// 请求的数据
	Buffer* m_pResponse;			// 返回的数据
	CURL* m_pEasy;					// 句柄
	struct curl_slist* m_pHeader;	// 头部数据
	char m_pError[CURL_ERROR_SIZE];	// 错误信息
};

typedef ObjectPool<EasyCurl> EasyCurlPool;

class MultiCurl : public RefObject, public TimerObject
{
public:
	friend class EasyCurl;
	typedef std::vector<uint32> Uint32Vector;
	typedef std::deque<RequestData*> RequestDataQueue;
public:
	MultiCurl(void);
	virtual ~MultiCurl(void);

	static int socketCallback(CURL *pEasy, curl_socket_t s, int what, void *userp, void *socketp);
	static int timeoutCallback(CURLM *pMulti, long timeout_ms, MultiCurl *pMultiCurl);

	// from TimerObject
	virtual int64 timerCallback(void);

	// 接收一个请求
	void acceptRequestWithTask(RequestData* pRequest);
	bool acceptRequest(RequestData* pRequest);
	inline EasyCurl* getEasyCurl(uint32 handle){
		return m_pEasyCurlPool->get(handle);
	}
	inline bool isBusy(void) const {
		return (this->getRequestCount() > 0);
	}
	inline int getRequestCount(void) const {
		return m_requestCount;
	}
	inline int getTimeoutCall(void) const {
		return m_timeoutCall;
	}
	inline void setEpollWorker(EpollWorker* pEpollWorker){ m_pEpollWorker = pEpollWorker; }
	inline EpollWorker* getEpollWorker(void){ return m_pEpollWorker; }
	inline void setEpoll(Epoll* pEpoll){ m_pEpoll = pEpoll; }
	inline Epoll* getEpoll(void){ return m_pEpoll; }
	void initialize(void);
	void destroy(void);
protected:
	bool requestWithEasyCurl(EasyCurl* pEasyCurl, RequestData* pRequest);
	inline bool queueRequest(RequestData* pRequest){
		if(m_requestQueue.size() >= MAX_REQUEST_QUEUE){
			return false;
		}
		pRequest->retain();
		m_requestQueue.push_back(pRequest);
		return true;
	}
	inline RequestData* getRequestQueue(void){
		if(m_requestQueue.size() > 0){
			RequestData* pRequest = m_requestQueue.front();
			m_requestQueue.pop_front();
			return pRequest;
		}
		return NULL;
	}
	inline void clearRequestQueue(void){
		for(auto pRequest : m_requestQueue){
			pRequest->release();
		}
		m_requestQueue.clear();
	}
	inline EasyCurl* createEasyCurl(void){
		if( this->getRequestCount() >= MAX_REQUEST_PERFORM ){
			return NULL;
		}
		EasyCurl* pEasyCurl = m_pEasyCurlPool->create();
		pEasyCurl->setMultiCurl(this);
		pEasyCurl->setEpoll(getEpoll());
		return pEasyCurl;
	}
	inline bool removeEasyCurl(EasyCurl* pEasyCurl){
		return this->removeEasyCurl(pEasyCurl->getHandle());
	}
	inline bool removeEasyCurl(uint32 handle){
		return m_pEasyCurlPool->idle(handle);
	}
	void responseRequest(EasyCurl* pEasyCurl, bool isRequestOK);
	void multiInfoRead(void);
	void mcodeCheck(const char *where, CURLMcode code);
	void removeAndClearEasy(EasyCurl* pEasyCurl);
	inline void onCurlEpollIn(EasyCurl* pEasyCurl){
		curl_multi_socket_action(m_pMulti, CURL_CSELECT_IN, pEasyCurl->getSocketFD(), &m_stillRunning);
	}
	inline void onCurlEpollOut(EasyCurl* pEasyCurl){
		curl_multi_socket_action(m_pMulti, CURL_CSELECT_OUT, pEasyCurl->getSocketFD(), &m_stillRunning);
	}
	inline bool multiRemove(EasyCurl* pEasyCurl){
		return ( CURLM_OK == curl_multi_remove_handle(m_pMulti, pEasyCurl->m_pEasy) );
	}
	inline bool multiAdd(EasyCurl* pEasyCurl){
		return ( CURLM_OK == curl_multi_add_handle(m_pMulti, pEasyCurl->m_pEasy) );
	}
protected:
	RequestDataQueue m_requestQueue;
	CURLM* m_pMulti;
	EasyCurlPool* m_pEasyCurlPool;
	EpollWorker* m_pEpollWorker;
	Epoll* m_pEpoll;
	int64 m_timeoutCall;
	int m_requestCount;
	int m_stillRunning;
};

class CurlResponseTask : public Task
{
public:
	CurlResponseTask(void) : Task(), m_pResponse(NULL), m_callbackID(0), m_isRequestOK(false) {}
	virtual ~CurlResponseTask(void){
		SAFE_RELEASE(m_pResponse)
	}

	// from Task
	virtual void doTask(Handler* pHandler){
		pHandler->onCurlResponse(m_pResponse, m_callbackID, m_isRequestOK);
	}
	virtual void doTask(ActiveWorker* pHandler);

	inline void setResponseData(Buffer* pData){
		SAFE_RETAIN(pData);
		SAFE_RELEASE(m_pResponse)
		m_pResponse = pData;
	}
public:
	Buffer* m_pResponse;			// 返回的数据
	uint32 m_callbackID;			// 回调ID
	bool m_isRequestOK;				// 请求是否成功
};

class CurlRequestTask : public Task
{
public:
	CurlRequestTask(void) : Task(), m_pRequest(NULL) {}
	virtual ~CurlRequestTask(void){
		SAFE_RELEASE(m_pRequest)
	}

	// from Task
	virtual void doTask(Handler* pHandler){}
	virtual void doTask(ActiveWorker* pHandler);

	inline void setRequestData(RequestData* pRequest){
		SAFE_RETAIN(pRequest);
		SAFE_RELEASE(m_pRequest)
		m_pRequest = pRequest;
	}
public:
	RequestData* m_pRequest;		// 请求的数据
};

NS_HIVE_END

#endif
