#include <iostream>
#include <map>
#include "useractHelper.h"
#include "common/util.h"

using namespace std;

string UserActHelper::getUsers(const Json::Value &root) {	// 获得所有用户名
	vector<string> keyname = {"username", "uuid"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	auto sqlRet = mSqlFunc.getUsers2User(root["username"].asString()) ;
	return Util::getRetStringMore(sqlRet.second, "users", 
			root["username"].asString()) ;
}
string UserActHelper::getUserInfo(const Json::Value &root) {		// 获得用户信息
	vector<string> keyname = {"username", "uuid","who"};
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	auto sqlRet = mSqlFunc.getUserInfo2User(root["who"].asString()) ;
	return Util::getRetStringMore(sqlRet.second, "userInfo", 
			root["username"].asString()) ;
}

string UserActHelper::updateUserInfo(const Json::Value &root) {		// 更改用户信息
	vector<string> keyname = {"username", "uuid","update"};
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	map<string, string> updateTable ;
	updateTable["avatar"] = "" ;
	updateTable["username"] = "" ;
	updateTable["phone"] = "" ;
	updateTable["password"] = "" ;
	updateTable["aboutMe"] = "" ;
	int arraySize = root["update"].size();
	bool hasUpdate = false ;
	for (int i = 0; i < arraySize; i++) {
		if(!(root["update"][i].isMember("key") &&
					root["update"][i].isMember("value")))
			return Util::getRetString(410,"request information is not complete") ;
		string key = root["update"][i]["key"].asString() ;
		string value = root["update"][i]["value"].asString() ;
		dbg_printf("key : %s\n", key.data()) ;
		dbg_printf("value : %s\n", value.data()) ;
		auto it = updateTable.find(key) ;
		if(it != updateTable.end()) {
			hasUpdate = true ;
			updateTable[key] = value ;
		}
	}
	if(hasUpdate) 
		mSqlFunc.updateUsrInfo2User(root["username"].asString(), updateTable) ;
	return Util::getRetString(200,"success") ;
}
