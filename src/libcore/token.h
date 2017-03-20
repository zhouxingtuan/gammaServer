//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/6/18
// Time: 下午3:18
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__token__
#define __hive__token__

#include <iostream>
#include <fstream>

#include "common.h"

NS_HIVE_BEGIN

class Token
{
public:
	typedef std::map<std::string, std::string> TokenMap;
protected:

public:
	Token(void){}
	virtual ~Token(void){}

	static void readConfig(const std::string& file, TokenMap& tokenMap);
	static void split(const std::string& str, const std::string& delimiter, TokenMap& tokenMap);
	static void trim(std::string& str);
	static void replace(std::string& str, const std::string& rp, const std::string& by);
};

NS_HIVE_END

#endif
