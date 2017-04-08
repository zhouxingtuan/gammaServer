//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/4/8
// Time: 上午11:54
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__handlercreator__
#define __hive__handlercreator__

#include "mainhandler.h"

NS_HIVE_BEGIN

Destination* HandlerCreatorCreateObject(Destination::index_type index);

void HandlerCreatorReleaseObject(Destination* pObj);

NS_HIVE_END

#endif
