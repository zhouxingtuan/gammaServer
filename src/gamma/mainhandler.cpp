//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/25
// Time: 下午11:12
// To change this template use File | Settings | File Templates.
//

#include "mainhandler.h"

NS_HIVE_BEGIN

void parseConfig(const char* fileName){
	MainWorker::getInstance()->initialize(1, 2, 2);
}

NS_HIVE_END
