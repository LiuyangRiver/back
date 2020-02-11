#ifndef _REALTIMEMSG_H_
#define _REALTIMEMSG_H_

#include <jsoncpp/json/json.h>
#include <iostream>
#include "mysql/sqlexecutor.h"
class RealTimeMsg
{
public:
	int addMsg(const Json::Value &root) ;
	/*
	 *	brief : 查询最近三天用户username收到的未读信息
	 *	ret : [{ "from" : "xxx", "time","xxx", "msg":"xxx" }]
	 *
	 */
	pair<int,Json::Value> getUnreadMsg(const string &username, 
			const uint64_t& lastMsgTime) ;
	int getUnreadMsgNum(const string &username, const uint64_t& lastMsgTime,
			const uint32_t &lastMsgId) ;
	int getNewUnreadMsg(const string &username, const uint64_t& lastMsgTime,
			const uint32_t& lastMsgId,Json::Value &data) ;

	pair<int,Json::Value> getRTMsgLog(const Json::Value &root ,
			const string &myAvatar, const string& peerAvatar) ;

private:
	/* data */
	SqlExecutor &mSqlExecutor = SqlExecutor::getInstance() ; 
};

#endif 
