//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 2017/5/10
// Time: 上午8:30
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__cppmodule__
#define __hive__cppmodule__

#include "core.h"

NS_HIVE_BEGIN

extern "C"{

Handler* HandlerCreateObject(uint32 index, uint32 poolType);
void HandlerReleaseObject(Handler* pObj);

void SOInitialize(void);
void SODestroy(void);

}

NS_HIVE_END

#endif
