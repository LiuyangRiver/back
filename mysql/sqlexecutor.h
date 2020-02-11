#ifndef _SQLEXECUTOR_H_
#define _SQLEXECUTOR_H_
#include <mysql/mysql.h>
#include <pthread.h>
#include "mysql/sqlutil.h"

using std::string ;
class SqlExecutor
{
public:
	~SqlExecutor();
	static SqlExecutor *getInstancePtr() ;
	static SqlExecutor &getInstance() ;

	int execSQL(const string &sql) ;
	int getResultNum() ;
	//MYSQL_RES *getResult() ;
	MYSQL_ROW getRow() ;

	void start();

private:
	static void* run(void* val) ;
	SqlExecutor();
private:
	/* data */

	static SqlExecutor *mSqlExecutor ;
	pthread_mutex_t sqlMutex ;
	MYSQL mysql ;
	MYSQL_ROW row;
    MYSQL_RES *result = NULL;
    MYSQL_FIELD *field;
	int sqlRet = 0;
	bool status = false ;
};

#endif 
