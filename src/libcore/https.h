//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/1/23
// Time: 下午6:27
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__https__
#define __hive__https__

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "http.h"


NS_HIVE_BEGIN

class Https : public Http
{
public:
	Https(void);
	virtual ~Https(void);

	static Destination* createObject(void){
		return new Https();
	}
	static void releaseObject(Destination* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	// from Http
	virtual int readSocket(void);
	virtual int writeSocket(void);

	bool bindSSL(SSL_CTX* ctx);
protected:
	// from Http
	virtual void resetData(void);

	void destroySSL(void);
protected:
	SSL *m_pSSL;
};


NS_HIVE_END

#endif
