#ifndef _APPROVAL_H_
#define _APPROVAL_H_

#include <iostream>
#include <jsoncpp/json/json.h>
#include "mysql/sqlexecutor.h"
class Approval
{
public:
	int addApproval(const Json::Value &root) ;
	int dealApproval(const Json::Value &root) ;
	/*
	 *	brief : 查询最近三天用户username收到的未读信息
	 *	ret : [{ "from" : "xxx", "time","xxx", "msg":"xxx" }]
	 *
	 */
	string getUnreadMsg(const string &username) ;

	/* 
	 * brief : 读取所有与用户相关的propose
	 * param : root 用户请求
	 *		   userInfo 用户相关信息
	 * ret : int :
	 *			0 无信息
	 *			1 读取成功
	 *			2 后续还有信息
	 *			-1 数据库操作错误
	 *		  json :
	 *
	 */
	pair<int, Json::Value> getApproval(const Json::Value &root, 
			const Json::Value &userInfo) ;
	pair<int, Json::Value> getApprovalById(const Json::Value &root) ;
	int getApprovalNum(const string &username, const uint64_t& lastApprovalTime,
			const int & lastApprovalId) ;
	int getNewApproval(const string &username, const uint64_t& lastApprovalTime,
			const int& lastApprovalId,Json::Value &data) ;

	int getInformNum(const uint64_t &lastInformTime, const int&lastInformId) ;
	pair<int, Json::Value> getInform(const Json::Value &root) ;
	int getNewInform(const string &username, const uint64_t& lastInformTime,
			const int& lastInformId,Json::Value &data) ;
private:
	//pair<int, Json::Value> loadData(const string& sql1,const string &sql2,
		//const string &username, const string& fileName) ;
	int loadNewData(const string& sql1,Json::Value &data , 
			const int& typeCode , const string &username, const uint64_t& lastTime,
			const uint32_t &lastId) ;

private:
	/* data */
	SqlExecutor &mSqlExecutor = SqlExecutor::getInstance() ; 
};

#endif 
