#include <iostream>
#include "user.h"
#include "common/util.h"

using namespace std;

pair<int,Json::Value> User::getUserInfo(const string &username) {
	Json::Value root ;
	string sql = "select * from user where (Username='"+username+"' )" ;
	int ret = mSqlExecutor.execSQL(sql) ;
	if(ret != 0) return make_pair(false, root) ;
	if(mSqlExecutor.getResultNum() > 0){
		MYSQL_ROW row = mSqlExecutor.getRow();
		root["username"] = row[1] ;
		root["prefix"] = row[2] ;
		root["level"] = atoi(row[3]) ;
		root["realName"] = row[5] ;
		root["phone"] = row[6] ;
		root["idcard"] = row[7] ;
		root["aboutMe"] = row[9] ;
		root["other"] = row[13] ;
		root["avatar"] = row[14] ;
		root["gender"] = atoi(row[15]) ;
		return make_pair(true,root) ;
	}
	return make_pair(false, root) ;
}

int User::addUser(const Json::Value& root) {
	vector<string> keys = {"Username", "Prefix" ,"Pubkey", "Password", "Realname",
	"Phone", "IDCard", "AboutMe", "Print", "Face", "Iris", "Other", "Level", "Gender"} ;
	vector<string> values ;
	uint32_t stringNum = keys.size()-2 ;
	uint32_t i = 0 ;
	for (i = 0; i < stringNum; i++) {
		values.push_back(root[keys[i]].asString()) ;
	}
	for (; i < keys.size(); i++) {
		values.push_back( to_string(root[keys[i]].asInt()) ) ;
	}
	string sql = getInsertSql(keys, values, "user", stringNum) ;
	dbg_printf("%s\n",sql.data()) ;
	int ret = mSqlExecutor.execSQL(sql) ;
	if(ret != 0) return 502 ;
	return ret ;
}
bool User::hasUser(const string &keyname, const string& value) {
	string sql = "select * from user where " ;
	sql = sql + keyname + " =\"" + value + "\"" ;
	int ret = mSqlExecutor.execSQL(sql) ;
	if(ret != 0) return false ;
	return (mSqlExecutor.getResultNum() > 0) ;
}

pair<bool,Json::Value> User::getUserByNamePwd(const string &username,
		const string& password) {
	Json::Value root ;
	string sql = "select * from user where (" ; 
	sql = sql + "Username=\""+username+"\" and password=\""+password+"\")" ;
	int ret = mSqlExecutor.execSQL(sql) ;
	if(ret != 0) return make_pair(false, root) ;
	if(mSqlExecutor.getResultNum() > 0){
		MYSQL_ROW row = mSqlExecutor.getRow();
		root["username"] = row[1] ;
		root["prefix"] = row[2] ;
		root["level"] = atoi(row[3]) ;
		root["realName"] = row[5] ;
		root["phone"] = row[6] ;
		root["idcard"] = row[7] ;
		root["aboutMe"] = row[9] ;
		root["other"] = row[13] ;
		root["avatar"] = row[14] ;
		root["gender"] = atoi(row[15]) ;
		return make_pair(true,root) ;
	}
	return make_pair(false, root) ;
}

pair<uint64_t,uint32_t> User::getLastInformTime(const string& username) {
	string sql = "select lastInformTime,lastInformId from user where username='" ;
	sql = sql + username + "'" ;
	mSqlExecutor.execSQL(sql) ;
	MYSQL_ROW row = mSqlExecutor.getRow();
	if(!row) return make_pair(-1,0) ;
	return make_pair(atol(row[0]), atoi(row[1])) ;
}

pair<uint64_t,uint32_t> User::getLastMsgTime(const string& username) {
	string sql = "select lastMsgTime,lastMsgId from user where username='" ;
	sql = sql + username + "'" ;
	mSqlExecutor.execSQL(sql) ;
	MYSQL_ROW row = mSqlExecutor.getRow();
	if(!row) return make_pair(-1,0) ;
	return make_pair(atol(row[0]), atoi(row[1])) ;
}

pair<uint64_t,uint32_t> User::getLastApprovalTime(const string& username) {
	string sql = "select lastApprovalTime,lastApprovalId from user where \
				  username='" ;
	sql = sql + username + "'" ;
	mSqlExecutor.execSQL(sql) ;
	MYSQL_ROW row = mSqlExecutor.getRow();
	if(!row) return make_pair(-1,0) ;
	return make_pair(atol(row[0]), atoi(row[1])) ;
}
pair<int,Json::Value> User::getUsers(const string& username) {
	string sql = "select username,Phone,AboutMe,avatar,Gender,realname from user" ;
	sql = sql + " where username != '" + username +"'" ;
	mSqlExecutor.execSQL(sql) ;
	Json::Value retJson ;
	MYSQL_ROW row ;
	int i = 0 ;
	while((row = mSqlExecutor.getRow()) ){
		Json::Value item ;
		item["username"] = row[0] ;
		item["phone"] = row[1] ;
		item["aboutMe"] = row[2] ;
		item["avatar"] = row[3] ;
		item["gender"] = atoi(row[4]) ;
		item["realName"] = row[5] ;
		retJson[i++] = item ;
	}
	return make_pair(1, retJson) ;
}

string User::getAvatar(const string &username) {
	string sql = "select avatar from user where username='" ;
	sql = sql + username + "'" ;
	mSqlExecutor.execSQL(sql) ;
	MYSQL_ROW row = mSqlExecutor.getRow();
	return row[0];
}
static inline string getUpdateUserInfoSql(const string&username , 
		const map<string,string>& updateTable){
	string sql = "update user set " ;
	string setStr = "" ;
	for(auto it = updateTable.begin(); it != updateTable.end(); it ++) {
		if( it->second.size() <= 0) continue ;
		if(setStr.size() > 0) setStr += "," + it->first+"='"+ it->second +"'" ;
		else setStr += it->first+"='"+ it->second +"'" ;
	}
	sql = sql + setStr + " where username='" + username + "'" ;
	return sql ;
}
int User::updateUsrInfo(const string &username, 
		const map<string,string>& updateTable) {
	string sql = getUpdateUserInfoSql(username, updateTable) ;
	dbg_printf("sql : %s\n",sql.data()) ;
	return mSqlExecutor.execSQL(sql) ;
}
