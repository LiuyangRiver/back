#include <iostream>
#include "approval.h"
#include "user.h"
#include "common/util.h"

using namespace std;


int Approval::addApproval(const Json::Value &root) {
	vector<string> keys = {"proposer", "approver" ,"detail","typeCode"} ;
	vector<string> values = {root["username"].asString(), 
		root["approver"].asString(), root["detail"].toStyledString(),
		to_string(root["typeCode"].asInt())};

	string sql = getInsertSql(keys, values, "approval", 3) ;
	int ret = mSqlExecutor.execSQL(sql) ;
	return ret ;
}

pair<int, Json::Value> Approval::getApprovalById(const Json::Value &root) {
	int id = root["approvalId"].asInt() ;
	//dbg_printf("%s\n",root.toStyledString().data()) ;
	string sql = "select id,proposer,approver,time,process,typeCode,detail,reason \
				  from approval where id="+to_string(id) ;
	Json::Value retJson ;
	int ret = mSqlExecutor.execSQL(sql) ;
	if(ret != 0 ) return make_pair(502, retJson) ;
	MYSQL_ROW row  = mSqlExecutor.getRow() ;
	if(!row) return make_pair(0, retJson) ;
	retJson["approvalId"] = atoi(row[0]) ;
	retJson["proposer"] = row[1] ;
	retJson["approver"] = row[2] ;
	retJson["time"] = (Json::UInt64)atol(row[3]) ;
	retJson["process"] = atoi(row[4]) ;
	retJson["typeCode"] = atoi(row[5]) ;
	retJson["detail"] = row[6] ;
	if(row[7]) retJson["reason"] = row[7] ;
	else retJson["reason"] = "" ;
	return make_pair(ret, retJson) ;

}

static inline string getGetApprovalSql(const Json::Value &root){
	int pageNo = root["pageNo"].asInt() ;
	int pageSize = root["pageSize"].asInt() ;
	int offset = pageSize*pageNo ;
	string username = root["username"].asString() ;
	int queryCode = root["queryCode"].asInt() ;
	int typeCode = -1 ;
	if(root.isMember("typeCode")){
		typeCode = root["typeCode"].asInt() ;
	}

	string sql1 = "select a.id,a.proposer,a.approver,a.time,a.process,a.typeCode,\
				   a.detail,a.reason, b.username,b.avatar,b.realName,\
				   b.phone,b.gender from approval a " ;
	string join = " join user b on a.approver=b.username ";
	if(queryCode == 1) join = " join user b on a.proposer=b.username ";
	string limitCond = " order by a.id desc limit " + to_string(pageSize) 
		+ " offset "+ to_string(offset);
	string whereCond = " where a.typeCode > 0 and " ;
	if(typeCode > 0) 
		whereCond = " where a.typeCode = '"+ to_string(typeCode)+ "' and " ;
	if(queryCode == 0) whereCond = whereCond + " a.proposer='"+username+"' " ;
	else if(queryCode == 1) whereCond = whereCond + " a.approver='"+username+"' " ;
	else return "" ;
	sql1 = sql1 + join + whereCond + limitCond ;
	return sql1 ;
}

pair<int, Json::Value> Approval::getApproval(const Json::Value &root,
		const Json::Value &userInfo) {
	Json::Value retJson ;
	string sql1 = getGetApprovalSql(root) ;
	if(sql1 == "") return make_pair(0, retJson) ;
	int ret = mSqlExecutor.execSQL(sql1) ;
	if(ret != 0 ) return make_pair(502, retJson) ;
	int rowNum = mSqlExecutor.getResultNum() ;
	if(rowNum <= 0) return make_pair(0, retJson) ;
	MYSQL_ROW row ;
	int retInt = 200 ;
	int i = 0;
	int queryCode = root["queryCode"].asInt() ;
	Json::Value userInfo1 ;
	userInfo1["username"] = userInfo["username"].asString() ;
	userInfo1["avatar"] = userInfo["avatar"].asString() ;
	userInfo1["realName"] = userInfo["realName"].asString() ;
	userInfo1["phone"] = userInfo["phone"].asString() ;
	userInfo1["gender"] = userInfo["gender"].asInt() ;
	while((row = mSqlExecutor.getRow()) ){
		Json::Value item ;
		item["approvalId"] = atoi(row[0]) ;
		//item["proposer"] = row[1] ;
		//item["approver"] = row[2] ;
		item["time"] = (Json::UInt64)atol(row[3]) ;
		item["process"] = atoi(row[4]) ;
		item["typeCode"] = atoi(row[5]) ;
		item["detail"] = row[6] ;
		if(row[7]) item["reason"] = row[7] ;
		else item["reason"] = "" ;
		Json::Value userInfo2 ;
		userInfo2["username"] = row[8];
		userInfo2["avatar"] = row[9];
		userInfo2["realName"] = row[10];
		userInfo2["phone"] = row[11];
		userInfo2["gender"] = atoi(row[12]);
		if(queryCode == 0) {
			item["proposer"] = userInfo1 ;
			item["approver"] = userInfo2 ;
		}else {
			item["proposer"] = userInfo2 ;
			item["approver"] = userInfo1 ;
		}
		retJson[i++] = item ;
	}
	return make_pair(retInt, retJson) ;
}


int Approval::getApprovalNum(const string &username, 
		const uint64_t& lastApprovalTime, const int& lastApprovalId) {
	string sql = "select id,proposer,approver,time,typecode,process,detail,\
				  reason from approval where ( typeCode!=0 and ((approver='" ; 
				  sql = sql + username+"' and process=0) or (proposer='" + username 
				  +"' and process>0)) and (time> "
				  +to_string(lastApprovalTime) +" or id > "+to_string(lastApprovalId)+"))";
	mSqlExecutor.execSQL(sql) ;
	return mSqlExecutor.getResultNum() ;
}

int Approval::getInformNum(const uint64_t &lastInformTime, 
		const int& lastInformId) {
	string sql = "select * from approval where typeCode=0 and (time > " ;
	sql = sql + to_string(lastInformTime) + " or id > '"+to_string(lastInformId)
		+"')" ;
	mSqlExecutor.execSQL(sql) ;
	return mSqlExecutor.getResultNum() ;
}

pair<int, Json::Value> Approval::getInform(const Json::Value &root) {
	Json::Value retJson ;
	int pageNo = root["pageNo"].asInt() ;
	int pageSize = root["pageSize"].asInt() ;
	int offset = pageSize*pageNo ;
	string sql1 = "select id,proposer,approver,time,process,typeCode,detail,\
				   reason from approval where typeCode=0 order by id desc " ;
	sql1 = sql1 + "limit " + to_string(pageSize) + " offset "+ to_string(offset);
	int ret = mSqlExecutor.execSQL(sql1) ;
	if(ret != 0 ) return make_pair(502, retJson) ;
	int rowNum = mSqlExecutor.getResultNum() ;
	if(rowNum <= 0) return make_pair(0, retJson) ;
	MYSQL_ROW row ;
	int retInt = 200 ;
	int i = 0;
	while((row = mSqlExecutor.getRow()) ){
		Json::Value item ;
		item["id"] = atoi(row[0]) ;
		item["proposer"] = row[1] ;
		item["approver"] = row[2] ;
		item["time"] = (Json::UInt64)atol(row[3]) ;
		item["process"] = atoi(row[4]) ;
		item["typeCode"] = atoi(row[5]) ;
		Json::Value detail ;
		Json::Reader reader ;
		reader.parse(row[6], detail) ;
		item["detail"] = detail ;
		if(row[7]) item["reason"] = row[7] ;
		else item["reason"] = "" ;
		retJson[i++] = item ;
	}
	return make_pair(retInt, retJson) ;
}


int Approval::dealApproval(const Json::Value &root) {
	string process = "2" ;
	int approvalRst = root["approvalRst"].asInt() ;
	if(approvalRst != 0) process  = "3" ;
	string sql = "update approval set reason='"  ;
	sql = sql + root["reason"].asString() + "',approvalRst=" 
		+ to_string(approvalRst) + ",process="+process + " where id=" 
		+ to_string(root["approvalId"].asInt()) +" and approver='"
		+ root["username"].asString()+"'";
	dbg_printf("%s\n",sql.data()) ;
	int ret = mSqlExecutor.execSQL(sql) ;
	return ret ;
}

int Approval::loadNewData(const string& sql1,Json::Value &data , 
		const int& typeCode , const string &username, const uint64_t& lastTime,
		const uint32_t &lastId) {
	int result = 0 ;
	int ret = mSqlExecutor.execSQL(sql1) ;
	if(ret != 0 ) return (data.toStyledString().size() > SLICE_DATA_MAX_SZ) ;
	int rowNum = mSqlExecutor.getResultNum() ;
	if(rowNum <= 0) return (data.toStyledString().size() > SLICE_DATA_MAX_SZ) ;

	MYSQL_ROW row;
	int i = 0 ;
	uint64_t maxTime = lastTime ;
	uint32_t maxId = lastId ;
	while((row = mSqlExecutor.getRow() )){
		Json::Value item ;
		item["typeCode"] = typeCode ;
		Json::Value itemContent ;
		uint32_t id = atoi(row[0]) ;
		uint64_t time = atol(row[3]) ;
		itemContent["approvalId"] = id;
		itemContent["proposer"] = row[1] ;
		itemContent["approver"] = row[2] ;
		itemContent["time"] = (Json::UInt64)time ;
		itemContent["typeCode"] = atoi(row[4]) ;
		itemContent["process"] = atoi(row[5]) ;
		Json::Value detail ;
		Json::Reader reader ;
		reader.parse(row[6], detail) ;
		if(typeCode == 1) itemContent["detail"] = detail ;
		else itemContent["detail"] = row[6] ;
		if(row[7]) itemContent["reason"] = row[7] ;
		else itemContent["reason"] = "" ;

		item["content"] = itemContent ;
		// 有问题！ 如果getNews一直不停，前面的文件会被覆盖
		if(item.toStyledString().size() > SLICE_DATA_MAX_SZ){   
			data[i++] = item ;
			if(time > maxTime) maxTime = time ;
			if(id > maxId) maxId = id ;
			break ;
		}else if(data.toStyledString().size()+item.toStyledString().size()
				> SLICE_DATA_MAX_SZ){
			result = 1;
			break ;
		}else{
			data[i++] = item ;
			if(time > maxTime) maxTime = time ;
			if(id > maxId) maxId = id ;
		}
	}
	string sql2= "" ;
	//if(maxId != 0 ){
	if((maxId > lastId && maxTime >= lastTime) ||
			(maxId >= lastId && maxTime > lastTime)){
		if(typeCode == 1) {  // 通知
			sql2 = "update user set lastInformTime ='" + to_string(maxTime) 
				+"',lastInformId=" +to_string(maxId) + " where username='" 
				+ username +"'" ;
		}else if(typeCode == 2) {// 审批
			sql2 = "update user set lastApprovalTime ='" + to_string(maxTime) 
				+"',lastApprovalId=" +to_string(maxId) + " where username='" 
				+ username +"'" ;
		}
		if(sql2 != "") mSqlExecutor.execSQL(sql2) ;
	} 
	return result ;
}
int Approval::getNewInform(const string &username, const uint64_t& lastInformTime,
		const int& lastInformId,Json::Value &data) {
	string sql1 = "select id,proposer,approver,time,typecode,process,detail,\
				   reason from approval where typeCode=0 and (time > " ;
	sql1 = sql1 + to_string(lastInformTime) +" or id>"+to_string(lastInformId) 
		+")" ;
	return loadNewData(sql1, data, 1,username, lastInformTime, lastInformId) ;
}

int Approval::getNewApproval(const string &username, 
		const uint64_t& lastApprovalTime, const int& lastApprovalId,
		Json::Value &data) {
	string sql1 = "select id,proposer,approver,time,typecode,process,detail,\
				   reason from approval where ( typeCode!=0 and ((approver='" ; 
				   sql1 = sql1 + username+"' and process=0) or (proposer='" + username 
				   +"' and process>0)) and (time> "
				   +to_string(lastApprovalTime) +" or id > "+to_string(lastApprovalId)+"))";
	return loadNewData(sql1, data, 2, username, lastApprovalTime, lastApprovalId);
}
