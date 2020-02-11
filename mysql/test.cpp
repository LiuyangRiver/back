#include <iostream>
#include <unistd.h>
#include "sqlexecutor.h"

using namespace std;

int main()
{
	SqlExecutor &mSqlExecutor = SqlExecutor::getInstance() ;

	string sql = "select * from user" ;
	while(1){
		mSqlExecutor.execSQL(sql) ;
		int num = mSqlExecutor.getResultNum() ;
		cout << "user num = " << num << endl ;
		sleep(1) ;
	}
	return 0;
}
