//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/2
// Time: 下午11:26
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__http__
#define __hive__http__

#include "http_parser.h"
#include "epoll.h"
#include "destination.h"
#include "packet.h"
#include "task.h"
#include "util.h"
#include "timer.h"

NS_HIVE_BEGIN

#define HTTP_STATE_IDLE 		0		// 空闲状态
#define HTTP_STATE_READ 		1		// 正在处理读状态
#define HTTP_STATE_READ_WAIT 	2		// 正在epoll中等待读
#define HTTP_STATE_READ_DONE 	3		// 已经读取完毕
#define HTTP_STATE_WAIT		 	4		// 正在挂起等待发送
#define HTTP_STATE_WRITE	 	5		// 正在处理写状态
#define HTTP_STATE_WRITE_WAIT 	6		// 正在epoll中等待写
#define HTTP_STATE_WRITE_DONE 	7		// 已经写入完毕
#define HTTP_STATE_DESTROY	 	8		// 已经被标记为删除，Handler执行时会返回

#define HTTP_PARSE_INIT 			0
#define HTTP_PARSE_MESSAGE_BEGIN 	1
#define HTTP_PARSE_URL 				2
#define HTTP_PARSE_HEADER_FIELD 	3
#define HTTP_PARSE_HEADER_VALUE 	4
#define HTTP_PARSE_HEADER_COMPLETE 	5
#define HTTP_PARSE_BODY 			6
#define HTTP_PARSE_MESSAGE_END	 	7

typedef struct HttpValueRecorder{
	int offset;
	int length;
	HttpValueRecorder(void) : offset(0), length(0){}
	~HttpValueRecorder(void){}
	inline void reset(void){
		offset = 0;
		length = 0;
	}
}HttpValueRecorder;

typedef struct HttpHeaderRecorder{
	HttpValueRecorder field;
	HttpValueRecorder value;
	HttpHeaderRecorder(void) : field(), value(){}
	~HttpHeaderRecorder(void){}
	inline void reset(void){
		field.reset();
		value.reset();
	}
}HttpHeaderRecorder;

class EpollWorker;

class Http : public EpollConnectObject, public TimerObject
{
public:
	friend class HttpManager;
	typedef std::vector<HttpHeaderRecorder> HttpHeaderRecorderVector;
public:
	Http(void);
	virtual ~Http(void);

	static EpollObject* createObject(uint32 index, uint32 poolType){
		return new Http();
	}
	static void releaseObject(EpollObject* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	static int onMessageBegin(http_parser* pParser);
	static int onHeadersComplete(http_parser* pParser);
	static int onMessageComplete(http_parser* pParser);
	static int onUrl(http_parser* pParser, const char* at, size_t length);
	static int onHeaderField(http_parser* pParser, const char* at, size_t length);
	static int onHeaderValue(http_parser* pParser, const char* at, size_t length);
	static int onBody(http_parser* pParser, const char* at, size_t length);

	int recordUrl(const char* at, size_t length);
	int recordHeaderField(const char* at, size_t length);
	int recordHeaderValue(const char* at, size_t length);
	int recordBody(const char* at, size_t length);

	// from EpollObject
	virtual bool epollActive(uint32 events);
	virtual void epollIn(void);
	virtual void epollOut(void);
	virtual void epollRemove(void);
	virtual void epollCheck(void){}

	// from Destination
	virtual void onReceivePacket(Packet* pPacket, Task* pTask);

	// from TimerObject
	virtual int64 timerCallback(void);

	void responseRequest(const char* ptr, uint32 length);	// 返回请求字符串
	void responseBegin(uint32 length);
	void responseAppend(const char* ptr, uint32 length);    // 追加一个请求字符串
	void responseEnd(void);

	virtual int readSocket(void);
	virtual int writeSocket(void);

	inline char getState(void) {
		return m_httpState;
	}
	inline void setState(char state) {
		if( HTTP_STATE_DESTROY != m_httpState ){
			m_httpState = state;
		}
	}
	inline void clearState(void) { m_httpState = HTTP_STATE_IDLE; }
	inline Buffer* createBuffer(int bufferSize){
		if(NULL == m_pBuffer){
			m_pBuffer = new Buffer(bufferSize);	// 是否处理分配内存失败？
			m_pBuffer->retain();
		}
		return m_pBuffer;
	}
	inline void releaseBuffer(void){ SAFE_RELEASE(m_pBuffer); }
	inline Buffer* getBuffer(void){ return m_pBuffer; }
	inline void setParseState(char state){ m_parseState = state; }
	inline char getParseState(void) const { return m_parseState; }
	inline char* getUrl(void) { return (m_pBuffer->data() + m_url.offset); }
	inline int getUrlLength(void) const { return m_url.length; }
	inline char* getBody(void) { return (m_pBuffer->data() + m_body.offset); }
	inline int getBodyLength(void) const { return m_body.length; }
	inline char* getBufferOffsetPtr(int offset) { return (m_pBuffer->data() + offset); }
	HttpHeaderRecorderVector& getHeaders(void) { return m_headers; }
	inline void setOffset(int offset) { m_offset = offset; }
	inline int getOffset(void) const { return m_offset; }
	inline void moveWriteOffset(int move) { m_offset += move; }
	inline char* getParseOffsetPtr(void) { return (m_pBuffer->data() + m_offset); }
	inline int getParseSize(void) const { return ((int)m_pBuffer->size() - getOffset()); }
	inline char* getWriteOffsetPtr(void) { return (m_pBuffer->data() + m_offset); }
	inline bool isWriteEnd(void) const { return (getOffset() >= (int)m_pBuffer->size()); }
	inline int getWriteSize(void) const { return ((int)m_pBuffer->size() - getOffset()); }
	inline void setIsInEpoll(bool isIn) { m_isInEpoll = isIn; }
	inline bool isInEpoll(void) const { return m_isInEpoll; }
	bool checkEpollState(uint32 events);
	inline void setEpollWorker(EpollWorker* pEpollWorker){ m_pEpollWorker = pEpollWorker; }
	inline EpollWorker* getEpollWorker(void){ return m_pEpollWorker; }
	virtual void resetData(void);
	void initialize(void);
	inline http_parser* getParser(void){ return &m_paeser; }
	inline int getOffsetAt(const char* at){ return (int)(at - m_pBuffer->data()); }
	void checkEpollRemove(void);
protected:
	http_parser m_paeser;
	EpollWorker* m_pEpollWorker;
	Buffer* m_pBuffer;
	char m_httpState;
	char m_parseState;
	bool m_isInEpoll;
	int m_offset;
	HttpValueRecorder m_url;
	HttpValueRecorder m_body;
	HttpHeaderRecorderVector m_headers;
};

NS_HIVE_END

#endif
