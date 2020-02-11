#ifndef _WHITELIST_H_
#define _WHITELIST_H_

#include "mysql/sqlexecutor.h"
#include <jsoncpp/json/json.h>

class WhiteList
{
public:
	WhiteList() ;

	pair<bool,Json::Value> getMsgByPhone(const string &phone) ;
	int addMsg(const string &phone, const string &realname, const string &IDcard);
private:
	SqlExecutor &mSqlExecutor = SqlExecutor::getInstance(); 
};

#endif 
