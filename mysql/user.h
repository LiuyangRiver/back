#ifndef _USER_H_
#define _USER_H_

#include <jsoncpp/json/json.h>
#include <iostream>
#include <map>
#include "mysql/sqlexecutor.h"

using std::string ;
using std::map ;
class User
{
public:
	int addUser(const Json::Value& root) ;
	bool hasUser(const string &keyname, const string& value) ;

	/*
	 * brief : 通过用户名和密码查询用户，并返回用户信息
	 * param : username 用户名， password 密码
	 *
	 * ret : bool 用户存在与否 
	 *		json 
	 *    root["username"]
	 *    root["prefix"] =
	 *    root["level"] = 
	 *    root["realname"]
	 *    root["phone"] = 
	 *    root["idcard"] =
	 *    root["aboutme"] 
	 *    root["other"] = 
	 */ 

	pair<bool,Json::Value> getUserByNamePwd(const string &username,
			const string& password) ;
	// ret (时间，ID)
	pair<uint64_t,uint32_t> getLastInformTime(const string& username) ;
	pair<uint64_t,uint32_t> getLastMsgTime(const string& username) ;
	pair<uint64_t,uint32_t> getLastApprovalTime(const string& username) ;

	pair<int,Json::Value> getUserInfo(const string &username) ;
	int updateUsrInfo(const string &username, const map<string,string> &updateTable) ;

	string getAvatar(const string &username) ;

	pair<int,Json::Value> getUsers(const string& username ) ;
private:
	/* data */
	SqlExecutor &mSqlExecutor = SqlExecutor::getInstance() ; 
};

#endif 
