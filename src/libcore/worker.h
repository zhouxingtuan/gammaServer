//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/29
// Time: 下午5:00
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__worker__
#define __hive__worker__

#include "handler.h"

NS_HIVE_BEGIN

class HandlerQueue;

class Worker : public RefObject, public Thread
{
public:
	explicit Worker(HandlerQueue* pHandlerQueue);
    virtual ~Worker(void);
    virtual int threadFunction(void);
protected:
	HandlerQueue* m_pHandlerQueue;
};// end class Worker

NS_HIVE_END

#endif
