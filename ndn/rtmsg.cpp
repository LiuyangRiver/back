#include <iostream>
#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/security/transform.hpp>
#include <jsoncpp/json/json.h>
#include "useractHelper.h"
#include "common/util.h"

using namespace std;

string UserActHelper::getRTMsg(const Json::Value &root){
	vector<string> keyname = {"username", "uuid"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	auto sqlRet = mSqlFunc.getUnreadMsg2RealTimeMsg(root["username"].asString());
	return Util::getRetStringMore(sqlRet.second, 
			root["username"].asString(), "rtmsg") ;
	
}
string UserActHelper::sendRTMsg(const Json::Value &root) {
	vector<string> keyname = {"from", "to", "uuid", "msg"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	mSqlFunc.addMsg2RealTimeMsg(root) ;
	return Util::getRetString(200, "success") ;
}


string UserActHelper::getRTMsgLog(const Json::Value &root) {
	
	vector<string> keyname = {"username", "uuid", "peer", "timestamp","limit",
	"direct"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;

	auto sqlRet = mSqlFunc.getRTMsgLog2RealTimeMsg(root) ;
	return Util::getRetStringMore(sqlRet.second, "rtmsgLog", 
			root["username"].asString()) ;
}
