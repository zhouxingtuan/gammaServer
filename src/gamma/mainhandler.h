//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/25
// Time: 下午11:12
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__mainhandler__
#define __hive__mainhandler__

#include "core.h"

NS_HIVE_BEGIN

void parseConfig(const char* fileName);

class MainHandler : public Handler
{
public:
	MainHandler(void);
	virtual ~MainHandler(void);


};

NS_HIVE_END

#endif
