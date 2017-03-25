//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/26
// Time: 上午8:02
// To change this template use File | Settings | File Templates.
//

#include "core.h"

//jemalloc
#define JEMALLOC_NO_DEMANGLE
#include "jemalloc.h"

// 重新定义C++的内存分配函数，由jemalloc来进行内存分配
void* operator new(size_t size){
	return je_malloc(size);
}
void* operator new(size_t size, const std::nothrow_t& nothrow_value){
	return je_malloc(size);
}
void operator delete(void* p){
	je_free(p);
}
void* operator new[](size_t size){
	return je_malloc(size);
}
void* operator new[](size_t size, const std::nothrow_t& nothrow_value){
	return je_malloc(size);
}
void operator delete[](void* p){
	je_free(p);
}

USING_NS_HIVE;

int main(int argc, char *argv[])
{
	// 使用默认的信号处理
	defaultSignalHandle();

	MainWorker::createInstance();
	MainWorker::getInstance()->initialize(1, 2, 2);
	MainWorker::getInstance()->update();
//MAIN_EXIT:
//	sleep(3);
	LOG_INFO("main loop exit ...");
	MainWorker::destroyInstance();

	return 0;
}