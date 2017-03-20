//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/11/22
// Time: 下午10:00
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__active__
#define __hive__active__

#include "epoll.h"
#include <sys/eventfd.h>

NS_HIVE_BEGIN

class Active : public EpollObject
{
public:
	Active(void);
	virtual ~Active(void);

	// from EpollObject
	virtual bool epollActive(uint32 events);
	virtual void epollIn(void){}
	virtual void epollOut(void){}
	virtual void epollRemove(void){}
	virtual void epollCheck(void){}

	virtual void onActive(uint64 value) = 0;

	inline bool active(void){
		uint64 value = 1;
		int nwrite = write(m_fd, &value, sizeof(uint64));
		if(nwrite != sizeof(uint64)){
			fprintf(stderr, "--Active::wakeup write failed\n");
			return false;
		}
//		fprintf(stderr, "--Active::wakeup m_fd write %llu\n", value);
		return true;
	}
protected:
	int readEvent(void);
	bool createEvent(Epoll* pEpoll);
	void closeEvent(void);
protected:

};

NS_HIVE_END

#endif
