#include <iostream>
#include <fstream>
#include <vector>
#include "sqlexecutor.h"
#include "common/util.h"

using namespace std;

SqlExecutor *SqlExecutor::mSqlExecutor = NULL;

SqlExecutor *SqlExecutor::getInstancePtr(){
	if(!mSqlExecutor) 
	{
		mSqlExecutor = new SqlExecutor() ;
		mSqlExecutor->start() ;
	}
	return mSqlExecutor ;
}
SqlExecutor &SqlExecutor::getInstance(){
	if(!mSqlExecutor) 
	{
		mSqlExecutor = new SqlExecutor() ;
		mSqlExecutor->start() ;
	}
	return *mSqlExecutor ;
}

static vector<string> getConfig(){
	vector<string> result ;
	string file_path = "./config.json" ;
	Json::Reader reader ;
	Json::Value root ;
	std::ifstream in(file_path.data(),ios::binary);
	if(!in.is_open()){
		dbg_printf("can not open file %s\n", file_path.data()) ;
		exit(1);
	}
	reader.parse(in,root);
	in.close();
	if((!root.isMember("mysqlUser")) || (!root.isMember("mysqlPasswd"))){
		dbg_printf("you must config mysqlUser or mysqlPasswd in %s\n", 
				file_path.data()) ; 
		exit(1) ;
	}
	result.push_back(root["mysqlUser"].asString()) ;
	result.push_back(root["mysqlPasswd"].asString()) ;
	return result ;
}

SqlExecutor::SqlExecutor(){

	vector<string> config = getConfig() ;
	string username = config[0] ;
	string passwd = config[1] ;

	mysql_init(&mysql) ;
	MYSQL * ret = mysql_real_connect(&mysql,"localhost",username.data(),
			passwd.data(), "minoa",3306,NULL,0) ;
	if(!ret){
		cout << "connect to MYSQL error" << endl ;
		throw "mysql exception" ;
	}
	//mysql_query(&mysql, "set names utf8");
	mysql_query(&mysql, "set names utf8mb4");
	pthread_mutex_init(&sqlMutex,NULL) ;
}

SqlExecutor::~SqlExecutor(){
	if(result) mysql_free_result(result) ;
	mysql_close(&mysql) ;
	mysql_library_end();		//解决 possible lost
}

int SqlExecutor::execSQL(const string &sql) {
	//dbg_printf("sql : %s\n", sql.data()) ;
	pthread_mutex_lock(&sqlMutex) ;
	if(result) mysql_free_result(result) ;
	result = NULL ;
	sqlRet = mysql_query(&mysql, sql.data());
	result = mysql_store_result(&mysql) ;
	pthread_mutex_unlock(&sqlMutex) ;
	//dbg_printf("sqlRet = %d result = %p\n",sqlRet, result) ;
	if(sqlRet != 0) dbg_perror("sql error : ") ;
	return sqlRet ;
}

int SqlExecutor::getResultNum(){
	//dbg_printf("sqlRet = %d result = %p\n",sqlRet, result) ;
	if(sqlRet != 0) return 0 ;		// SQL命令执行不成功
	else if(!result) return 0 ;
	//dbg_printf("mysql_num_rows(result) = %llu\n", mysql_num_rows(result)) ;
	return mysql_num_rows(result) ;
}

MYSQL_ROW SqlExecutor::getRow() {
	row = NULL  ;
	if(result) row = mysql_fetch_row(result) ;
	return row ;
}

void SqlExecutor::start(){
	if(status) return ;
	status = true ;
	pthread_t mtid ;
	pthread_create(&mtid,NULL,run,this) ;
}

void* SqlExecutor::run(void* val) {
	SqlExecutor *_this = (SqlExecutor*)val ;
	dbg_printf("SqlExecutor thread start %d\n",1) ;
	string sql = "select id from user where id<0" ;
	while(_this->status){
		sleep(1800) ;
		int sqlRet = _this->execSQL(sql) ;
		dbg_printf("prevent mysql connection from breaking down %d\n", sqlRet);
		if(sqlRet != 0 ) {
			cout << "connect to database broken down " << endl ;
		}
	}
	return NULL ;
}
