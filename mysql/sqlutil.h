#ifndef _SQLUTIL_H_
#define _SQLUTIL_H_

#include <iostream>
#include <vector>
#include <jsoncpp/json/json.h>

using std::string ;
using std::vector ;
using std::pair ;

pair<string,string> getKeyValueList(const vector<string>& key, 
		const vector<string>& value) ;

pair<string,string> getKeyValueList(const vector<string>& key, 
		const vector<string>& value, int stringNum) ;

/*
 * brief : 生成一条插入语句
 * param : 
 *		keys  表列名称
 *		values  对应的值列表
 * tablename : 表名
 * stringNum : 值列表中，属性是string的个数
 *
 *
 */
string getInsertSql(const vector<string>& keys, 
		const vector<string>& values, const string& tablename, int stringNum = -1);

void write2Cache(const Json::Value &more, const string& username, 
		const string &fileName) ;
#endif 
