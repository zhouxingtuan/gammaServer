//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午3:09
// To change this template use File | Settings | File Templates.
//

#include "buffer.h"

NS_HIVE_BEGIN

Buffer::Buffer(int length) : RefObject(), CharVector(length, 0) {
	clearEncryptFlag();
	this->clear();
}
Buffer::~Buffer(void){

}

NS_HIVE_END
