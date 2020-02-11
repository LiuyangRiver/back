#include <iostream>
#include "sqlfunc.h"
#include "common/util.h"

using namespace std;

// ============================== whitelist
pair<bool,Json::Value> SqlFunc::getMsgByPhone2Whitelist(const string &phone) {
	return mwhiteList.getMsgByPhone(phone) ;
}
int SqlFunc::addMsg2Whilelist(const string &phone, const string &realname,  
		const string &IDcard) {
	return mwhiteList.addMsg(phone, realname, IDcard) ;
}

// ============================== user
int  SqlFunc::addUser2User(const Json::Value& root) {
	return muser.addUser(root) ;
}
bool SqlFunc::hasUser2User(const string &keyname, const string& value) {
	return muser.hasUser(keyname, value) ;
}
pair<bool,Json::Value> SqlFunc::getUserByNamePwd2User(const string &username,
		const string& password) {
	return muser.getUserByNamePwd(username, password) ;
}
pair<int, Json::Value> SqlFunc::getUsers2User(const string& username) {
	return muser.getUsers(username) ;
}
pair<int, Json::Value> SqlFunc::getUserInfo2User(const string& username) {
	auto userInfo = muser.getUserInfo(username) ;
	userInfo.second.removeMember("idcard") ;
	return userInfo ;
}
int SqlFunc::updateUsrInfo2User(const string& username, 
		const map<string,string> &updateTable) {
	return muser.updateUsrInfo(username, updateTable) ;
}

// ============================== RealTimeMsg
int SqlFunc::addMsg2RealTimeMsg(const Json::Value &root) {
	return mrealTimeMsg.addMsg(root) ;
}
pair<int, Json::Value> SqlFunc::getUnreadMsg2RealTimeMsg(const string &username) {
	auto lastInfo = muser.getLastMsgTime(username) ;
	if(lastInfo.first <= 0) return make_pair(0,nullptr) ;
	return mrealTimeMsg.getUnreadMsg(username, lastInfo.first) ;
}
int SqlFunc::getUnreadMsgNum2RealTimeMsg(const string &username) {
	auto lastInfo = muser.getLastMsgTime(username) ;
	if(lastInfo.first <= 0) return 0;
	return mrealTimeMsg.getUnreadMsgNum(username, lastInfo.first, 
			lastInfo.second) ;
}
int SqlFunc::getNewUnreadMsg2RealTimeMsg(const string& username, 
		Json::Value &data) {
	auto lastInfo = muser.getLastMsgTime(username) ;
	if(lastInfo.first <= 0) return 0;
	return mrealTimeMsg.getNewUnreadMsg(username, lastInfo.first, 
			lastInfo.second, data) ;
}
pair<int,Json::Value> SqlFunc::getRTMsgLog2RealTimeMsg(const Json::Value &root) {
	string myAvatar = muser.getAvatar(root["username"].asString()) ;
	string peerAvatar = muser.getAvatar(root["peer"].asString()) ;
	if(myAvatar.size() <= 0 || peerAvatar.size() <= 0) 
		return make_pair(0,nullptr) ;
	return mrealTimeMsg.getRTMsgLog(root, myAvatar, peerAvatar) ;
}

// ============================== Approval
int SqlFunc::addApproval2Approval(const Json::Value &root) {
	return mApproval.addApproval(root) ;
}

/* 
 * ret : 成功返回0, 失败返回错误码
 *
 */
pair<int, Json::Value> SqlFunc::getApproval2Approval(const Json::Value &root) {
	auto userInfo = muser.getUserInfo(root["username"].asString()) ;
	return mApproval.getApproval(root, userInfo.second) ;
}
pair<int, Json::Value> SqlFunc::getApprovalById2Approval(const Json::Value& root){
	dbg_printf("%s\n",root.toStyledString().data()) ;
	Json::Value approvalJson = mApproval.getApprovalById(root).second ;
	if(approvalJson.isNull()) return make_pair(0,approvalJson) ;
	Json::Value proposerUserInfo = 
		muser.getUserInfo(approvalJson["proposer"].asString()).second;
	Json::Value approverUserInfo = 
		muser.getUserInfo(approvalJson["approver"].asString()).second;
	dbg_printf("=====================%d\n",1) ;

	Json::Value j1 ;
	j1["username"] = proposerUserInfo["username"].asString() ;
	j1["avatar"]   = proposerUserInfo["avatar"].asString() ;
	j1["realName"] = proposerUserInfo["realName"].asString() ;
	j1["phone"]    = proposerUserInfo["phone"].asString() ;
	j1["gender"]   = proposerUserInfo["gender"].asInt() ;
	approvalJson["proposer"] = j1 ;

	j1["username"] = approverUserInfo["username"].asString() ;
	j1["avatar"]   = approverUserInfo["avatar"].asString() ;
	j1["realName"] = approverUserInfo["realName"].asString() ;
	j1["phone"]    = approverUserInfo["phone"].asString() ;
	j1["gender"]   = approverUserInfo["gender"].asInt() ;
	approvalJson["approver"] = j1 ;

	return make_pair(0,approvalJson);
}
int SqlFunc::getApprovalNum2Approval(const string &username) {
	auto lastInfo = muser.getLastApprovalTime(username) ;
	if(lastInfo.first <= 0) return 0;
	return mApproval.getApprovalNum(username, lastInfo.first, lastInfo.second) ;
}
int SqlFunc::getInformNum2Approval(const string &username) {
	auto lastInfo = muser.getLastInformTime(username) ;
	if(lastInfo.first <= 0) return 0;
	return mApproval.getInformNum(lastInfo.first,lastInfo.second) ;
}
pair<int, Json::Value> SqlFunc::getInform2Approval(const Json::Value &root) {
	return mApproval.getInform(root) ;
}
int SqlFunc::dealApproval2Approval(const Json::Value &root) {
	return mApproval.dealApproval(root) ;
}
int SqlFunc::getNewApproval2Approval(const string& username, Json::Value &data) {
	auto lastInfo = muser.getLastApprovalTime(username) ;
	if(lastInfo.first <= 0) return 0;
	return mApproval.getNewApproval(username,lastInfo.first, lastInfo.second,
			data) ;
}
int SqlFunc::getNewInform2Approval(const string &username, Json::Value &data) {
	auto lastInfo = muser.getLastInformTime(username) ;
	if(lastInfo.first <= 0) return 0;
	return mApproval.getNewInform(username,lastInfo.first, lastInfo.second,
			data) ;
}

