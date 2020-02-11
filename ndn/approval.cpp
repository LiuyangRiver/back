#include <iostream>
#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/security/transform.hpp>
#include <jsoncpp/json/json.h>
#include "useractHelper.h"
#include "common/util.h"

using namespace std;

string UserActHelper::sendApproval(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid", "approver", "typeCode", 
		"detail"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	int sqlRet = mSqlFunc.addApproval2Approval(root) ;
	if(sqlRet != 0) return Util::getRetString(502,"OA database error") ;
	return Util::getRetString(200,"success", chkRet.second) ;
}

string UserActHelper::getApproval(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid", "queryCode", "pageNo", 
		"pageSize"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;

	auto sqlRet = mSqlFunc.getApproval2Approval(root) ;
	return Util::getRetStringMore(sqlRet.second ,"approval", 
			root["username"].asString()) ;
}
string UserActHelper::getApprovalById(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid", "approvalId" } ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	auto sqlRet = mSqlFunc.getApprovalById2Approval(root) ;
	return Util::getRetStringMore(sqlRet.second ,"approval", 
			root["username"].asString()) ;
}

string UserActHelper::getInform(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid", "pageNo", "pageSize"};
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	auto sqlRet = mSqlFunc.getInform2Approval(root) ;
	return Util::getRetStringMore(sqlRet.second ,"inform", 
			root["username"].asString()) ;
}

string UserActHelper::dealApproval(const Json::Value &root) {	// 处理审批
	vector<string> keyname = {"username", "uuid","reason", "approvalId", 
		"approvalRst"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;

	dbg_printf("%s\n",root.toStyledString().data()) ;
	int sqlRet = mSqlFunc.dealApproval2Approval(root) ;
	if(Util::isSqlErr(sqlRet)) Util::getRetString(502,"OA database error") ;
	return Util::getRetString(200,"success") ;
}

