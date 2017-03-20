//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午3:09
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__buffer__
#define __hive__buffer__

#include "common.h"

NS_HIVE_BEGIN

typedef std::vector<char> CharVector;

class Buffer : public RefObject, public CharVector
{
protected:
	std::atomic_flag m_encryptFlag;
public:
	explicit Buffer(int length);
	virtual ~Buffer(void);

	inline int write(const void* ptr, int length, int offset){
		size_t needSize = offset + length;
		if( this->size() < needSize ){
			this->resize(needSize, 0);
		}
		char* writeTo = (char*)(this->data()) + offset;
		memcpy(writeTo, ptr, length);
		return length;
	}
	inline int read(void* ptr, int length, int offset){
		int needSize = offset + length;
		if( (int)this->size() < needSize){
			return 0;
		}
		char* readFrom = (char*)(this->data()) + offset;
		memcpy(ptr, readFrom, length);
		return length;
	}
	template<typename T>
	inline T to(int offset) const {
		return *((T*)(this->data()+offset));
	}
	inline bool checkEncryptFlag(void){ return !m_encryptFlag.test_and_set(); }
	inline void clearEncryptFlag(void){ m_encryptFlag.clear(); }
};// end class Buffer

NS_HIVE_END

#endif
