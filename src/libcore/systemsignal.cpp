//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/18
// Time: 上午9:19
// To change this template use File | Settings | File Templates.
//

#include <map>
#include <string>
#include <stdio.h>
#include "systemsignal.h"

NS_HIVE_BEGIN

typedef struct SignalHandleInfo {
	std::string name;
	SignalCallback callback;
	bool quit;
}SignalHandleInfo;
typedef std::map<int, SignalHandleInfo> SignalHandleInfoMap;
static SignalHandleInfoMap g_signalMap;

void dumpCore(int sig) {
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    signal(sig, SIG_DFL);

    size = backtrace (array, 10);
    strings = (char **)backtrace_symbols (array, size);

    fprintf(stderr, "Stack trace:\n");
    for (i = 0; i < size; i++) {
        fprintf(stderr, "%d %s \n",i,strings[i]);
    }

    free (strings);
    exit(1);
}
void handleSignal(int sig){
	fprintf(stderr, "handleSignal called sig=%d\n", sig);
	SignalHandleInfoMap::iterator itCur = g_signalMap.find(sig);
	if( itCur != g_signalMap.end() ){
		fprintf(stderr, "signal name=%s\n", itCur->second.name.c_str());
		if( NULL != itCur->second.callback ){
			itCur->second.callback(sig);
		}
		if( itCur->second.quit ){
			fprintf(stderr, "handleSignal exit sig=%d name=%s\n", sig, itCur->second.name.c_str());
			exit(1);
		}
	}else{
		fprintf(stderr, "handleSignal unregister sig=%d\n", sig);
		exit(1);
	}
}

void signalExit(int sig, const char* name, SignalCallback cb){
	SignalHandleInfo info;
	info.name = name;
	info.callback = cb;
	info.quit = true;
	g_signalMap[sig] = info;
	signal(sig, handleSignal);
}
void signalHandle(int sig, const char* name, SignalCallback cb){
	SignalHandleInfo info;
	info.name = name;
	info.callback = cb;
	info.quit = false;
	g_signalMap[sig] = info;
	signal(sig, handleSignal);
}

// SIGKILL 和 SIGSTOP 无法捕获，也不能被忽略
void defaultSignalHandle(SignalCallback coreCb, SignalCallback exitCb, SignalCallback handleCb){
	// 忽略某些信号的处理
	SIGNAL_IGNORE(SIGPIPE);
	SIGNAL_IGNORE(SIGCHLD);

	if(NULL == coreCb){
		coreCb = dumpCore;
	}
	// 指针错误信号监听
	SIGNAL_EXIT(SIGSEGV, coreCb);
	// 监听并打印信号或处理
	SIGNAL_EXIT(SIGILL, exitCb);
	SIGNAL_EXIT(SIGTRAP, exitCb);
	SIGNAL_EXIT(SIGABRT, exitCb);
	SIGNAL_EXIT(SIGBUS, exitCb);
	SIGNAL_EXIT(SIGFPE, exitCb);
	SIGNAL_EXIT(SIGIOT, exitCb);
	SIGNAL_EXIT(SIGINT, exitCb);
	SIGNAL_EXIT(SIGQUIT, exitCb);
	SIGNAL_EXIT(SIGXCPU, exitCb);
	SIGNAL_EXIT(SIGXFSZ, exitCb);
	SIGNAL_EXIT(SIGTERM, exitCb);

	// 只监听打印，不处理
	SIGNAL_HANDLE(SIGALRM, handleCb);
	SIGNAL_HANDLE(SIGHUP, handleCb);
	SIGNAL_HANDLE(SIGPOLL, handleCb);
	SIGNAL_HANDLE(SIGPROF, handleCb);
	SIGNAL_HANDLE(SIGSYS, handleCb);
	SIGNAL_HANDLE(SIGVTALRM, handleCb);
	SIGNAL_HANDLE(SIGUSR1, handleCb);
	SIGNAL_HANDLE(SIGUSR2, handleCb);
}

NS_HIVE_END
