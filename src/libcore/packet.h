//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午2:57
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__packet__
#define __hive__packet__

#include "buffer.h"
#include "destination.h"

NS_HIVE_BEGIN

// 16777216 - 8 - 1
#define PACKET_DATA_MAX 16777207
#define PACKET_MAX_LENGTH 16777215

#define PACKET_HEAD_LENGTH 8

// 服务间传递消息的头部数据结构
typedef struct PacketHead {
	unsigned int length 	: 24;	// 数据长度
	unsigned int command 	: 8;	// 当前数据包的命令
	DestinationHandle destination;	// 4 目标服务句柄
} PacketHead;

#define WRITE_VALUE(p, v)\
	p->write(&v, sizeof(v))

#define READ_VALUE(p,v)\
	p->read(&v, sizeof(v))

class Packet : public RefObject
{
protected:
	Buffer* m_pBuffer;		// 数据指针
	int m_cursor;			// 数据读取的偏移
public:
    explicit Packet(int bufferSize);
    explicit Packet(Buffer* pBuffer);
    virtual ~Packet(void);

	inline void setCursorToEnd(void){ m_cursor = getLength(); }
	inline void setCursor(int cur){ m_cursor = cur; }
	inline void moveCursor(int length){ m_cursor += length; }
	inline int getCursor(void) const { return m_cursor; }
	inline void resetCursor(void){ m_cursor = 0; }
	inline Buffer* getBuffer(void){ return m_pBuffer; }
	inline PacketHead* getHead(void){ return (PacketHead*)(getDataPtr()); }	// 该指针在write调用后可能会变化
	inline char* getOffsetPtr(int offset){ return m_pBuffer->data() + offset; }
	inline char* getCursorPtr(void){ return m_pBuffer->data() + m_cursor; }
	inline char* getDataPtr(void){ return m_pBuffer->data(); }
	inline int getLength(void) const { return (int)m_pBuffer->size(); }
	inline bool isCursorEnd(void) const { return getCursor() >= getLength(); }
	inline char* getBody(void){ return getOffsetPtr(sizeof(PacketHead)); }
	inline int getBodyLength(void) const { return (getLength() - sizeof(PacketHead)); }

	inline void readBegin(void){
		setCursor((int)sizeof(PacketHead));
	}
	inline void readEnd(void){
		resetCursor();
	}
	inline void writeBegin(unsigned int cmd, unsigned int handle){
		m_pBuffer->resize(sizeof(PacketHead), 0);
		setCursor((int)sizeof(PacketHead));
		setCommand(cmd);
		setDestination(handle);
	}
	inline void writeEnd(void){
		recordLength();
	}
	inline int write(const void* ptr, int length){
		int n = m_pBuffer->write(ptr, length, getCursor());
		if( n > 0 ){
			moveCursor(n);
		}
		return n;
	}
	inline int read(void* ptr, int length){
		int n = m_pBuffer->read(ptr, length, getCursor());
		if( n > 0 ){
			moveCursor(n);
		}
		return n;
	}
	inline void setCommand(unsigned int cmd){ getHead()->command = cmd; }
	inline void setDestination(unsigned int handle){ getHead()->destination = handle; }
	inline void recordLength(void){ getHead()->length = getLength(); }
	inline unsigned int getCommand(void) { return getHead()->command; }
	inline unsigned int getDestination(void) { return getHead()->destination.handle; }
};// end class Packet

NS_HIVE_END

#endif
