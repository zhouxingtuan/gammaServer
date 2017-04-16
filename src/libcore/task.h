//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/16
// Time: 下午11:59
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__task__
#define __hive__task__

#include "common.h"

NS_HIVE_BEGIN

class Handler;
class ActiveWorker;

class Task : public RefObject
{
public:
	Task(void) : RefObject() {}
	virtual ~Task(void){}

	virtual void doHandlerTask(Handler* pHandler) = 0;
	virtual void doActiveTask(ActiveWorker* pHandler) = 0;
};

NS_HIVE_END

#endif
