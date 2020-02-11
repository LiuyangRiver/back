#include <iostream>
#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/security/transform.hpp>
#include <jsoncpp/json/json.h>
#include "useractHelper.h"
#include "common/util.h"


using namespace std;

static int createUserDir(const string& username){
	dbg_printf("username = %s\n", username.data()) ;
	string pwd = getenv("PWD") ;
	string filePath = "./files/" ;
	filePath += username;
	if(access(filePath.data(), F_OK) < 0) mkdir(filePath.data(), 0755) ;
	string cachePath = filePath + "/cache" ;
	if(access(cachePath.data(), F_OK) < 0) mkdir(cachePath.data(), 0755) ;

	string actualPath = "../share" ;
	string symPath = "./files/"+username+"/share" ;
	if(access(symPath.data(), F_OK) < 0) 
		symlink(actualPath.data(), symPath.data())  ;
	return 0 ;
}

string UserActHelper::getVCode(const Json::Value &root) {
	string vcode = "123456" ;
	return Util::getRetString(200, "success", vcode) ;
}


string UserActHelper::registry(const Json::Value &root){
	vector<string> keyname = {"command", "level", "password", "phone",
		"publicKey", "username"} ;
	if(!chkJsonKeyIntegrity(keyname, root)) {
		return Util::getRetString(410, "request message is not completed");
	}
	string phone = root["phone"].asString();
	string username = root["username"].asString();
	auto sqlRet = mSqlFunc.getMsgByPhone2Whitelist(phone) ;
	dbg_printf("%d\n",sqlRet.first) ;
	if(!sqlRet.first) {
		return Util::getRetString(405, "user are not in whitelist") ;
	}else if(mSqlFunc.hasUser2User("Phone", phone)) {
		return Util::getRetString(405, "phone already exist") ;
	}else if(mSqlFunc.hasUser2User("Username", username)) {
		return Util::getRetString(405, "username already exist") ;
	}
	return doRegistry(root, sqlRet.second) ;
}

string UserActHelper::doRegistry(const Json::Value &root, 
		const Json::Value &userInfo) {

	string prefix = "/" ;
	prefix += root["username"].asString() ;
	dbg_printf("register %s \n", prefix.data()) ;

	Json::Value bcJson ;
	bcJson["Type"		] = "user-act";
	bcJson["Command"	] = "Registry";
	bcJson["Pubkey"		] = root["publicKey"].asString();
	bcJson["Prefix"		] = prefix;
	bcJson["Level"		] = root["level"].asInt();
	bcJson["Timestamp"	] = to_string(Util::getCurTimestamp() );
	bcJson["Username"	] = root["username"].asString();
	bcJson["Realname"	] = userInfo["realname"].asString() ;
	bcJson["Phone"		] = root["phone"].asString();
	bcJson["IDcard"		] = userInfo["idCard"].asString();
	bcJson["AboutMe"	] = "xxx";
	bcJson["Face"		] = "xxx";
	bcJson["Print"		] = "xxx";
	bcJson["Iris"		] = "xxx";
	bcJson["Other"		] = "xxx";
	bcJson["Sig"		] = "xxx";
	auto bcRet = mDnsClient.registry(bcJson.toStyledString()) ;
	if(bcRet.first < 0){
		return Util::getRetString(501, bcRet.second) ;
	}
	dbg_printf("%s \n", bcRet.second.data()) ;
	bcJson["Password"] = root["password"].asString() ;
	bcJson["Gender"] = userInfo["gender"].asInt() ;
	int insertRet = mSqlFunc.addUser2User(bcJson) ;
	if(insertRet > 300) return Util::getRetString(502,"register fail") ;
	createUserDir(root["username"].asString()) ;

	return Util::getRetString(200,"Registry Success!") ;
}
