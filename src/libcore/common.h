//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午12:53
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__common__
#define __hive__common__

#define NS_HIVE_BEGIN namespace HiveNS {
#define NS_HIVE_END };
#define USING_NS_HIVE using namespace HiveNS;

//c libs
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>  //atof函数
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>

//std libs
#include <atomic>
#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>

//epoll & net libs
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>		/* basic system data types */
#include <sys/socket.h>		/* basic socket definitions */
#include <netinet/in.h>		/* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>		/* inet(3) functions */
#include <sys/epoll.h>		/* epoll function */
#include <fcntl.h>			/* nonblocking */

//thread libs
#include <pthread.h>
#include <semaphore.h>

#define SAFE_DELETE(ptr) if(ptr != NULL){ delete ptr; ptr = NULL; }
#define SAFE_DELETE_ARRAY(ptr) if(ptr != NULL){ delete [] ptr; ptr = NULL; }
#define SAFE_RETAIN(ptr) if(ptr != NULL){ ptr->retain(); }
#define SAFE_RELEASE(ptr) if(ptr != NULL){ ptr->release(); ptr = NULL; }

NS_HIVE_BEGIN

// 统一所有数据类型的使用
typedef char 						int8;
typedef short 						int16;
typedef int 						int32;
typedef long long int				int64;
typedef unsigned char 				uint8;
typedef unsigned short  			uint16;
typedef unsigned int    			uint32;
typedef unsigned long long int  	uint64;

#define MAX_INT64 9223372036854775807
#define MIN_INT64 -9223372036854775808
#define MAX_INT32 2147483647
#define MIN_INT32 -2147483648
/*--------------------------------------------------------------------*/
// 拥有同步互斥量的类
class Sync
{
protected:
	pthread_mutex_t m_mutex;
public:
	Sync(void){
		pthread_mutex_init(&m_mutex, NULL);
	}
	virtual ~Sync(void){
		pthread_mutex_destroy(&m_mutex);
	}
	inline void lock(void){
		pthread_mutex_lock(&m_mutex);
	}
	inline void rlock(void){
		pthread_mutex_lock(&m_mutex);
	}
	inline void unlock(void){
		pthread_mutex_unlock(&m_mutex);
	}
};
/*--------------------------------------------------------------------*/
// 引用计数基类

class RefObject
{
protected:
	std::atomic<int> m_referenceCount;
public:
	RefObject(void) : m_referenceCount(0){}
	virtual ~RefObject(void){}
	inline void release(void){
		if( std::atomic_fetch_sub_explicit(&m_referenceCount, 1, std::memory_order_relaxed) == 1 ){
			delete this;
		}
	}
	inline void retain(void){
		std::atomic_fetch_add_explicit(&m_referenceCount, 1, std::memory_order_relaxed);
	}
	inline int getRefCount(void){ return (int)m_referenceCount; }
};
/*--------------------------------------------------------------------*/
// 线程基类
class Thread
{
public:
	typedef void*(*ThreadCallback)(void* pData);
protected:
	pthread_t m_pThread;
public:
	Thread(void) : m_pThread(0){}
	virtual ~Thread(void){}
	bool startThread(void);
	void cancelThread(void);
	virtual int threadFunction(void) = 0;

	static pthread_t staticThread(ThreadCallback start_rtn, void *arg);
private:
	static void* staticThreadFunction(void* pData);
};
/*--------------------------------------------------------------------*/

NS_HIVE_END

#endif
