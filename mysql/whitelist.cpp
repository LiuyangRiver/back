#include <iostream>
#include "whitelist.h"
#include "common/util.h"

using namespace std;

WhiteList::WhiteList(){
	//this->mSqlExecutorPtr = SqlExecutor::getInstancePtr() ;
	//dbg_printf("mSqlExecutorPtr = %p \n", mSqlExecutorPtr) ;
}

pair<bool,Json::Value> WhiteList::getMsgByPhone(const string &phone) {
	string sql = "select Realname,Gender,IDCard from whitelist where Phone = '" ;
	sql = sql + phone + "'" ;
	dbg_printf("sql = %s\n",sql.data()) ;
	int ret = mSqlExecutor.execSQL(sql) ;
	dbg_printf("sql ret = %d\n", ret) ;
	Json::Value root ;
	if(ret != 0) return make_pair(false, root) ;
	dbg_printf(" mSqlExecutor.getResultNum()= %d\n", mSqlExecutor.getResultNum()) ;
	if( mSqlExecutor.getResultNum() <= 0) return make_pair(false,root) ;
	MYSQL_ROW row = mSqlExecutor.getRow() ;
	root["realname"] = row[0] ;
	root["gender"] = atoi(row[1]) ;
	root["idCard"] = row[2] ;
	return make_pair(true, root);
}

int  WhiteList::addMsg(const string &phone, const string &realname, 
		const string &IDcard){
	string sql = "insert into whitelist " ;
	vector<string> key = {"Phone", "Realname", "IDcard"} ;
	vector<string> value = {phone, realname, IDcard} ;
	auto lists = getKeyValueList(key, value) ;
	sql = sql + lists.first + " values " + lists.second ;
	int ret= 0 ;
	//int ret = mSqlExecutor.execSQL(sql) ;
	return ret ;
}
