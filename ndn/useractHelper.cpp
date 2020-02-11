#include <iostream>
#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/security/transform.hpp>
#include <jsoncpp/json/json.h>
#include <unistd.h>
#include <sys/stat.h>
#include "useractHelper.h"
#include "common/util.h"

using namespace std;


UserActHelper * UserActHelper::mUserActHelper ;

UserActHelper* UserActHelper::getInstancePtr() {
	if(mUserActHelper == NULL) mUserActHelper = new UserActHelper() ;
	return mUserActHelper ;
}
UserActHelper& UserActHelper::getInstance() {
	if(mUserActHelper == NULL) mUserActHelper = new UserActHelper() ;
	return *mUserActHelper ;
}

UserActHelper::UserActHelper(){
	mLoginTablePtr = LoginTable::getInstancePtr() ;
	mLoginTablePtr->start() ;
	actionMap["/register"] = &UserActHelper::registry ;
	actionMap["/login"] = &UserActHelper::login ;
	actionMap["/getVCode"] = &UserActHelper::getVCode ;
	actionMap["/getUsers"] = &UserActHelper::getUsers ;
	actionMap["/userInfo"] = &UserActHelper::getUserInfo ;
	actionMap["/updateUserInfo"] = &UserActHelper::updateUserInfo ;

	actionMap["/sendRTMsg"] = &UserActHelper::sendRTMsg ;
	actionMap["/getRTMsg"] = &UserActHelper::getRTMsg ;
	actionMap["/sendApproval"] = &UserActHelper::sendApproval ;
	actionMap["/getApproval"] = &UserActHelper::getApproval ;
	actionMap["/getApprovalById"] = &UserActHelper::getApprovalById ;
	actionMap["/dealApproval"] = &UserActHelper::dealApproval ;

	actionMap["/fileInfo"] = &UserActHelper::fileInfo ;
	actionMap["/downloadFile"] = &UserActHelper::downloadFile;
	actionMap["/getDataSlice"] = &UserActHelper::getDataSlice;
	actionMap["/fileAction"] = &UserActHelper::fileAction;

	actionMap["/getVCode"] = &UserActHelper::getVCode ;
	actionMap["/hasNews"] = &UserActHelper::hasNews ;
	actionMap["/getNews"] = &UserActHelper::getNews ;
	actionMap["/getInform"] = &UserActHelper::getInform ;
	actionMap["/getRTMsgLog"] = &UserActHelper::getRTMsgLog ;
}

UserActHelper::~UserActHelper(){
	delete mLoginTablePtr ;
	delete mUserActHelper ;
}

string UserActHelper::dealReq(const string &requestStr){
	Json::Value root ;
	Json::Reader reader ;
	try{
		reader.parse(requestStr , root) ;
	}catch(std::exception e){
		return Util::getRetString(408, "json parse exception") ;
	}
	if(!chkJsonKeyIntegrity({"command"}, root)) 
		return Util::getRetString(410,"request message is not completed");
	string cmd = root["command"].asString() ;
	dbg_printf("cmd = %s\n", cmd.data()) ;
	//dbg_printf("requestStr : %s\n", requestStr.data()) ;
	auto it = actionMap.find(cmd) ;
	if(it != actionMap.end()){
		return (this->*(it->second))(root) ;
	} 
	return "" ;
}

string UserActHelper::dealReq(const string &requestStr, const char* buf, 
		const int bufSize) {
	Json::Value root ;
	Json::Reader reader ;
	try{
		reader.parse(requestStr , root) ;
	}catch(std::exception e){
		return Util::getRetString(408, "json parse exception") ;
	}
	if(!chkJsonKeyIntegrity({"command"}, root)) 
		return Util::getRetString(410,"request message is not completed");
	string cmd = root["command"].asString() ;
	dbg_printf("cmd = %s\n", cmd.data()) ;

	return uploadFile(root, buf, bufSize) ;
}

string UserActHelper::hasNews(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid"};
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;

	string username = root["username"].asString() ;
	int msgNum = mSqlFunc.getUnreadMsgNum2RealTimeMsg(username) ;
	int approvalNum = mSqlFunc.getApprovalNum2Approval(username) ;
	int informNum = mSqlFunc.getInformNum2Approval(username);
	Json::Value retJson ;
	retJson["msg"] = msgNum ;
	retJson["approval"] = approvalNum ;
	retJson["inform"] = informNum ;
	return Util::getRetString(200, "success", retJson) ;
}
string UserActHelper::getNews(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid"};
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;

	string username = root["username"].asString() ;
	Json::Value data ;
	int ret = mSqlFunc.getNewUnreadMsg2RealTimeMsg(username, data) ;
	if(ret != 1) ret = mSqlFunc.getNewInform2Approval(username, data) ;
	if(ret != 1) ret = mSqlFunc.getNewApproval2Approval(username, data) ;

	return Util::getRetStringMore(data, "news", username) ;
}

bool UserActHelper::chkJsonKeyIntegrity(const vector<string> &keyname,
		const Json::Value& root){
	for (uint32_t i = 0; i < keyname.size(); i++) {
		if(!root.isMember(keyname[i]) || root[keyname[i]].isNull()) return false ;
	}
	return true ;
}

pair<bool,string> UserActHelper::chkIntAndLogin(const vector<string> &keyname, 
		const Json::Value &root ){
	if(!chkJsonKeyIntegrity(keyname,root)) return make_pair(false, 
			Util::getRetString(410,"request message is not completed"));
	string username = root[keyname[0]].asString() ;
	string uuid = root["uuid"].asString() ;
	if(!mLoginTablePtr->isLogin(username, uuid)) {
		return make_pair(false,Util::getRetString(407,
					"user did not logined yet") );
	}
	return make_pair(true,"") ;
}

string UserActHelper::login(const Json::Value &root) {
	string username = root["username"].asString() ;
	string password = root["password"].asString() ;
	if(!mSqlFunc.hasUser2User("Username", username)) {
		return Util::getRetString(406, "username are not exist") ;
	}
	auto sqlRet = mSqlFunc.getUserByNamePwd2User(username, password) ;
	if(sqlRet.first == false){
		return Util::getRetString(406, "password error") ;
	}else{
		dbg_printf("add to login table ============== %d\n",1) ;
		string uuid = mLoginTablePtr->addLogin(username) ;
		(sqlRet.second)["uuid"] = uuid ;
		cout << "new login uuid : " << uuid << endl ;
		return Util::getRetString(200, "login success", sqlRet.second);
	}
}

