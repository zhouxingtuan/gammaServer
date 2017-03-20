//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/2/26
// Time: 下午2:00
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__destination__
#define __hive__destination__

#include "common.h"

NS_HIVE_BEGIN

typedef struct DestinationHandle {
	union{
		struct{
//			unsigned int 	version 	: 8;	// 版本
			unsigned int 	node 		: 8;	// 节点
			unsigned int 	service 	: 5;	// 服务
			unsigned int 	type 		: 3;	// 类型
			unsigned int 	index 		: 16;	// 下标
		};
		unsigned int 		handle 		: 32;	// 句柄
	};
public:
	DestinationHandle(unsigned int h){ this->handle = h; }
	DestinationHandle(void) : handle(0) {}
	~DestinationHandle(void){}

	inline unsigned int getService(void) const { return this->service; }
	inline unsigned int getType(void) const { return this->type; }
	inline unsigned int getIndex(void) const { return this->index; }
//	inline unsigned int getVersion(void) const { return this->version; }
	inline unsigned int getNode(void) const { return this->node; }
	inline unsigned int getHandle(void) const { return this->handle; }
	inline void setService(unsigned int s){ this->service = s; }
	inline void setType(unsigned int t){ this->type = t; }
	inline void setIndex(unsigned int index){ this->index = index; }
	inline void setNode(unsigned int node){ this->node = node; }
//	inline void increaseVersion(void){ ++(this->version); }
	inline bool operator==(unsigned int h) const { return (this->handle == h); }
	inline bool operator==(const DestinationHandle& h) const{ return (this->handle == h.handle); }
	inline bool operator<(unsigned int h) const { return (this->handle < h); }
	inline bool operator<(const DestinationHandle& h) const { return (this->handle < h.handle); }
	inline DestinationHandle& operator=(unsigned int h){ this->handle = h; return *this; }
	inline DestinationHandle& operator=(const DestinationHandle& h){ this->handle = h.handle; return *this; }
}DestinationHandle;

class Packet;
class Task;

class Destination
{
public:
	typedef uint32 handle_type;
//	typedef uint32 version_type;
	typedef uint32 node_type;
	typedef uint32 index_type;
	typedef uint32 type_type;
protected:
	DestinationHandle m_handle;
public:
	Destination(void) : m_handle(0) {}
	virtual ~Destination(void){}

	inline unsigned int getService(void) const { return m_handle.getService(); }
	inline unsigned int getType(void) const { return m_handle.getType(); }
	inline unsigned int getIndex(void) const { return m_handle.getIndex(); }
//	inline unsigned int getVersion(void) const { return m_handle.getVersion(); }
	inline unsigned int getNode(void) const { return m_handle.getNode(); }
	inline unsigned int getHandle(void) const { return m_handle.getHandle(); }
	inline void setService(unsigned int s){ m_handle.setService(s); }
	inline void setType(unsigned int t){ m_handle.setType(t); }
	inline void setIndex(unsigned int index){ m_handle.setIndex(index); }
	inline void setNode(unsigned int node){ m_handle.setNode(node); }
//	inline void increaseVersion(void){ m_handle.increaseVersion(); }
	inline void setHandle(unsigned int handle) { m_handle = handle; }

	// 目标接收到packet
	virtual void onReceivePacket(Packet* pPacket, Task* pTask) = 0;
};

NS_HIVE_END

#endif
