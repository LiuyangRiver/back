#include <iostream>
#include "realTimeMsg.h"
#include "common/util.h"

using namespace std;

int RealTimeMsg::addMsg(const Json::Value &root) {
	vector<string> keys = {"FromName", "ToName" ,"Msg", "status"} ;
	vector<string> values = {root["from"].asString(), root["to"].asString(), 
		root["msg"].asString(), "0"};
	string sql = getInsertSql(keys, values, "realtimeMsg", keys.size()-1) ;
	int ret = mSqlExecutor.execSQL(sql) ;
	return ret ;
}
pair<int,Json::Value> RealTimeMsg::getUnreadMsg(const string &username, 
		const uint64_t &lastMsgTime) {
	string sql = "select * from realtimeMsg where ( time >= " ;
	sql = sql + to_string(lastMsgTime) + " and ToName='"+username +"' and status = 0)" ;

	mSqlExecutor.execSQL(sql) ;
	Json::Value retJson ;
	if(mSqlExecutor.getResultNum() <= 0) return make_pair(0,retJson) ;
	MYSQL_ROW row;
	int i = 0 ;
	while((row = mSqlExecutor.getRow() )){
		Json::Value item ;
		item["from"] = row[0] ;
		item["msg"] = row[2] ;
		item["time"] = row[3] ;
		retJson[i++] = item ;
	}

	sql = "update user set time =(select max(time) from realtimeMsg \
		   where ( time >=  " ;
	sql = sql + to_string(lastMsgTime) +" and ToName='"+username
		+"' and status=0)) where username='" + username +"'" ;
	mSqlExecutor.execSQL(sql) ;
	sql = "update realtimeMsg set status = 1 where ( time > ";
	sql = sql + to_string(lastMsgTime) + " and ToName='"+username +"' and status = 0)" ;
	mSqlExecutor.execSQL(sql) ;

	return make_pair(1, retJson);
}

int RealTimeMsg::getUnreadMsgNum(const string &username, 
		const uint64_t &lastMsgTime, const uint32_t &lastMsgId){
	string sql = "select * from realtimeMsg where ( (time > " ;
	sql = sql + to_string(lastMsgTime) + " or id > '"+to_string(lastMsgId)
		+"') and ToName='"+username +"')" ;
	mSqlExecutor.execSQL(sql) ;
	return mSqlExecutor.getResultNum() ;
}


// 返回1表示data装不下了
int RealTimeMsg::getNewUnreadMsg(const string &username, 
		const uint64_t& lastMsgTime,const uint32_t &lastMsgId, Json::Value &data) {
	int result = 0;
	string sql = "SELECT a.id,a.FromName,a.Msg,a.time,b.avatar from realtimeMsg a \
				  inner join user b on a.FromName=b.username where ( \
						  (a.time > ";
	sql += to_string(lastMsgTime )+ " or a.id > "+to_string(lastMsgId)+")"
		+ " and a.ToName='"+username +"')" ;
	//dbg_printf("===================%s\n",sql.data()) ;
	mSqlExecutor.execSQL(sql) ;
	if(mSqlExecutor.getResultNum() <= 0) return 0 ;
	MYSQL_ROW row;
	int i = 0 ;
	uint64_t maxTime = lastMsgTime ;
	uint32_t maxId = lastMsgId ;
	while((row = mSqlExecutor.getRow() )){
		Json::Value item ;
		item["typeCode"] = 0 ;
		Json::Value itemContent ;
		uint64_t time = atol(row[3]) ;
		itemContent["time"] = (int)time ;
		itemContent["msg"] = row[2] ;
		uint32_t msgId = atoi(row[0]) ;
		itemContent["id"] = msgId;

		Json::Value from ;
		from["username"] = row[1] ;
		from["avatar"] = row[4] ;
		itemContent["from"] = from ;
		item["content"] = itemContent ;
		if(item.toStyledString().size() > 7000) {
			data[i++] = item ;
			if(time > maxTime) maxTime = time ;
			if(msgId > maxId) maxId = msgId ;
			 result = 1;
			break ;
		}else if(data.toStyledString().size()
				+ item.toStyledString().size() > 7000){
			 result = 1;
			 break ;
		}else{
			data[i++] = item ;
			if(time > maxTime) maxTime = time ;
			if(msgId > maxId) maxId = msgId ;
		}
	}
	if((maxId > lastMsgId && maxTime >= lastMsgTime) || 
			(maxId >= lastMsgId && maxTime > lastMsgTime)){
		sql = "update user set lastMsgTime=" + to_string(maxTime) +",lastMsgId="
			+to_string(maxId) + " where username='" + username +"'" ;
		mSqlExecutor.execSQL(sql) ;
	}
	return result ;
	
}
pair<int,Json::Value> RealTimeMsg::getRTMsgLog(const Json::Value &root ,
		const string &myAvatar, const string& peerAvatar) {
	Json::Value retJson ;
	string myName = root["username"].asString() ;
	string peerName = root["peer"].asString() ;
	uint64_t time = root["timestamp"].asUInt64() ;
	int direct = root["direct"].asInt() ;
	int limit = root["limit"].asInt() ;
	string sql ;
	if(direct == 0){
		sql = "select id,FromName,ToName,Msg,time from realtimeMsg where ( \
			   (( FromName='"+myName+ "' and ToName='"+peerName
			   +"' )or (FromName='"+peerName+"' and ToName='"+myName
			   +"') ) and time >="+to_string(time)+") order by id limit "
			   + to_string(limit);
	}else{
		sql = "select id,FromName,ToName,Msg,time from realtimeMsg where ( \
			   (( FromName='"+myName+ "' and ToName='"+peerName
			   +"' )or (FromName='"+peerName+"' and ToName='"+myName
			   +"') ) and time <"+to_string(time)
			   +") order by id desc limit " + to_string(limit) ;
	}
	if(sql.size() <= 0) return make_pair(0,retJson) ;
	mSqlExecutor.execSQL(sql) ;
	int resultNum = mSqlExecutor.getResultNum() ;
	if(limit > resultNum ) limit = resultNum ;
	if(resultNum <= 0) return make_pair(0,retJson) ;
	MYSQL_ROW row;
	for (int i = 0; i < limit; i++) {
		row = mSqlExecutor.getRow() ;
		Json::Value item ;
		Json::Value from ;
		Json::Value to ;
		item["id"] = atoi(row[0]) ;
		string fromName = row[1] ;
		string toName = row[2] ;
		item["msg"] = row[3] ;
		item["time"] = (Json::UInt64)atol(row[4]) ;
		
		if(fromName == myName){
			from["username"] = myName ;
			from["avatar"] = myAvatar ;
			to["username"] = peerName ;
			to["avatar"] = peerAvatar ;
		}else{
			from["username"] = peerName ;
			from["avatar"] = peerAvatar ;
			to["username"] = myName ;
			to["avatar"] = myAvatar ;
		}
		item["from"] = from ;
		item["to"] = to ;
		retJson[i] = item ;
	}
	return make_pair(0,retJson) ;
}
