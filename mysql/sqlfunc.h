#ifndef _SQLFUNC_H_
#define _SQLFUNC_H_

#include <iostream>
#include <map>

#include "mysql/whitelist.h"
#include "mysql/user.h"
#include "mysql/realTimeMsg.h"
#include "mysql/approval.h"

using std::map ;
using std::string ;

class SqlFunc
{
public:
	/* 
	 * brief : 读取所有与用户相关的propose
	 * param : 用户请求json
	 * ret : bool :
	 *			0 无信息
	 *			1 有信息
	 *	
	 */
	pair<bool,Json::Value> getMsgByPhone2Whitelist(const string &phone) ;
	int addMsg2Whilelist(const string &phone, const string &realname,  
			const string &IDcard) ;

	int addUser2User(const Json::Value& root) ;
	bool hasUser2User(const string &keyname, const string& value) ;
	pair<bool,Json::Value> getUserByNamePwd2User(const string &username,
			const string& password) ;
	pair<int, Json::Value> getUsers2User(const string& username) ;
	pair<int, Json::Value> getUserInfo2User(const string& username) ;
	int updateUsrInfo2User(const string& username, const map<string,string> &) ;

	int addMsg2RealTimeMsg(const Json::Value &root) ;
	pair<int,Json::Value> getUnreadMsg2RealTimeMsg(const string &username) ;
	int getUnreadMsgNum2RealTimeMsg(const string &username) ;
	int getNewUnreadMsg2RealTimeMsg(const string& username, Json::Value &data) ;
	pair<int,Json::Value> getRTMsgLog2RealTimeMsg(const Json::Value &root) ;


	int addApproval2Approval(const Json::Value &root) ;
	/* 
	 * brief : 读取approval历史记录
	 * param : json 查询参数
	 * ret : int :
	 *			0 无信息
	 *			1 读取成功
	 *			2 后续还有信息
	 *			-1 数据库操作错误
	 *		  json :
	 *
			 {
				 "proposer"    :   "xxx" ,
				 "approver"    : "xxx" ,
				 "time"        : "xxx" ,
				 "status"      : 0 等待处理，1已处理
				 "typeCode"    :  0 出差；1 报销；2通知
				 "detail"    : {          服务器不关心该字段的具体值
				 "reason" : "xxx" ,  出差事由
				 "trafficTool" : "xxx",  交通工具
				 ...
				 }
			 }
	 */
	pair<int, Json::Value> getApproval2Approval(const Json::Value&) ;
	pair<int, Json::Value> getApprovalById2Approval(const Json::Value&) ;
	int getNewApproval2Approval(const string& username, Json::Value &data) ;
	int getApprovalNum2Approval(const string &username) ;
	int dealApproval2Approval(const Json::Value &root) ;

	pair<int, Json::Value> getInform2Approval(const Json::Value &root) ;
	int getNewInform2Approval(const string &username, Json::Value &data) ;
	int getInformNum2Approval(const string &username) ;
private:
	/* data */
	WhiteList mwhiteList ;
	User muser ;
	RealTimeMsg mrealTimeMsg ;
	Approval mApproval ;

};

#endif 
