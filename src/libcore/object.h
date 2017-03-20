//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/4
// Time: 下午12:49
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__object__
#define __hive__object__

#include "common.h"

NS_HIVE_BEGIN

#define INVALID_UNIQUE_HANDLE 0

#define MAX_1616_HASH_SIZE 65535
#define MAX_0824_HASH_SIZE 16777215

class Object1616
{
public:
	typedef uint32 handle_type;
	typedef uint32 version_type;
	typedef uint32 index_type;
	typedef struct Handle{
		union{
			struct{
				version_type 	version 	: 16;	// 版本
				index_type 		index 		: 16;	// 下标
			};
			handle_type 		handle 		: 32;	// 句柄
		};
	public:
		Handle(handle_type h){ this->handle = h; }
		Handle(void) : handle(0) {}
		~Handle(void){}

		inline index_type getIndex(void) const { return this->index; }
		inline version_type getVersion(void) const { return this->version; }
		inline handle_type getHandle(void) const { return this->handle; }
		inline void setIndex(index_type index){ this->index = index; }
		inline void increaseVersion(void){ ++(this->version); }
		inline bool operator==(handle_type h) const { return (this->handle == h); }
		inline bool operator==(const Handle& h) const{ return (this->handle == h.handle); }
		inline bool operator<(handle_type h) const { return (this->handle < h); }
		inline bool operator<(const Handle& h) const { return (this->handle < h.handle); }
		inline Handle& operator=(handle_type h){ this->handle = h; return *this; }
	}Handle;
public:
	Object1616(void) : m_handle(0) {}
	virtual ~Object1616(void){}

	inline index_type getIndex(void) const { return m_handle.getIndex(); }
	inline version_type getVersion(void) const { return m_handle.getVersion(); }
	inline handle_type getHandle(void) const { return m_handle.getHandle(); }
	inline void setIndex(index_type index) { return m_handle.setIndex(index); }
	inline void increaseVersion(void) { m_handle.increaseVersion(); }
	inline void setHandle(handle_type handle) { m_handle = handle; }
protected:
	Handle m_handle;
};

class Object0824
{
public:
	typedef uint32 handle_type;
	typedef uint32 version_type;
	typedef uint32 index_type;
	typedef struct Handle{
		union{
			struct{
				version_type 	version 	: 8;	// 版本
				index_type 		index 		: 24;	// 下标
			};
			handle_type 		handle 		: 32;	// 句柄
		};
	public:
		Handle(handle_type h){ this->handle = h; }
		Handle(void) : handle(0) {}
		~Handle(void){}

		inline index_type getIndex(void) const { return this->index; }
		inline version_type getVersion(void) const { return this->version; }
		inline handle_type getHandle(void) const { return this->handle; }
		inline void setIndex(index_type index){ this->index = index; }
		inline void increaseVersion(void){ ++(this->version); }
		inline bool operator==(handle_type h) const { return (this->handle == h); }
		inline bool operator==(const Handle& h) const{ return (this->handle == h.handle); }
		inline bool operator<(handle_type h) const { return (this->handle < h); }
		inline bool operator<(const Handle& h) const { return (this->handle < h.handle); }
		inline Handle& operator=(handle_type h){ this->handle = h; return *this; }
	}Handle;
public:
	Object0824(void) : m_handle(0) {}
	virtual ~Object0824(void){}

	inline index_type getIndex(void) const { return m_handle.getIndex(); }
	inline version_type getVersion(void) const { return m_handle.getVersion(); }
	inline handle_type getHandle(void) const { return m_handle.getHandle(); }
	inline void setIndex(index_type index) { return m_handle.setIndex(index); }
	inline void increaseVersion(void) { m_handle.increaseVersion(); }
	inline void setHandle(handle_type handle) { m_handle = handle; }
protected:
	Handle m_handle;
};

class Object080816
{
public:
	typedef uint32 handle_type;
	typedef uint32 version_type;
	typedef uint32 index_type;
	typedef uint32 type_type;
	typedef struct Handle{
		union{
			struct{
				version_type 	version 	: 8;	// 版本
				type_type 		type 		: 8;	// 版本
				index_type 		index 		: 16;	// 下标
			};
			handle_type 		handle 		: 32;	// 句柄
		};
	public:
		Handle(handle_type h){ this->handle = h; }
		Handle(void) : handle(0) {}
		~Handle(void){}

		inline type_type getType(void) const { return this->type; }
		inline index_type getIndex(void) const { return this->index; }
		inline version_type getVersion(void) const { return this->version; }
		inline handle_type getHandle(void) const { return this->handle; }
		inline void setType(type_type t){ this->type = t; }
		inline void setIndex(index_type index){ this->index = index; }
		inline void increaseVersion(void){ ++(this->version); }
		inline bool operator==(handle_type h) const { return (this->handle == h); }
		inline bool operator==(const Handle& h) const{ return (this->handle == h.handle); }
		inline bool operator<(handle_type h) const { return (this->handle < h); }
		inline bool operator<(const Handle& h) const { return (this->handle < h.handle); }
		inline Handle& operator=(handle_type h){ this->handle = h; return *this; }
	}Handle;
public:
	Object080816(void) : m_handle(0) {}
	virtual ~Object080816(void){}

	inline type_type getType(void) const { return m_handle.getType(); }
	inline index_type getIndex(void) const { return m_handle.getIndex(); }
	inline version_type getVersion(void) const { return m_handle.getVersion(); }
	inline handle_type getHandle(void) const { return m_handle.getHandle(); }
	inline void setType(type_type t) { m_handle.setType(t); }
	inline void setIndex(index_type index) { m_handle.setIndex(index); }
	inline void increaseVersion(void) { m_handle.increaseVersion(); }
	inline void setHandle(handle_type handle) { m_handle = handle; }
protected:
	Handle m_handle;
};

NS_HIVE_END

#endif
