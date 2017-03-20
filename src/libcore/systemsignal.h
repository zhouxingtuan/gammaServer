//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/18
// Time: 上午9:18
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__systemsignal__
#define __hive__systemsignal__

#include <signal.h>
#include <execinfo.h>
#include "common.h"

NS_HIVE_BEGIN

// 外部定义的信号处理函数
typedef void (*SignalCallback)(int sig);

extern void signalExit(int sig, const char* name, SignalCallback cb);
extern void signalHandle(int sig, const char* name, SignalCallback cb);
extern void defaultSignalHandle(SignalCallback coreCb=NULL, SignalCallback exitCb=NULL, SignalCallback handleCb=NULL);

#define SIGNAL_EXIT(SIG, FUNC)\
signalExit(SIG, #SIG, FUNC);

#define SIGNAL_HANDLE(SIG, FUNC)\
signalHandle(SIG, #SIG, FUNC);

#define SIGNAL_IGNORE(SIG)\
signal(SIG, SIG_IGN);

NS_HIVE_END

#endif
