#include <iostream>
#include "mysql/sqlutil.h"
#include "common/util.h"

using namespace std;

string getInsertSql(const vector<string>& keys, 
		const vector<string>& values, const string& tablename, int stringNum){

	if(stringNum <= 0 ) stringNum = keys.size() ;
	dbg_printf("%d\n",stringNum) ;
	string sql = "insert into " ;
	auto lists = getKeyValueList(keys, values, stringNum) ;
	sql = sql + tablename + " " + lists.first + " values " + lists.second ;
	dbg_printf("%s\n", sql.data()) ;
	return sql ;
}
pair<string,string> getKeyValueList(const vector<string>& key, 
		const vector<string>& value){
	string keyList = "(" ;
	string valueList = "(" ;
	for (uint32_t i = 0; i < key.size(); i++) {
		if(i > 0){
			keyList += "," ;
			valueList += "," ;
		}
		keyList += key[i] ;
		valueList = valueList + "'" + value[i] + "'";
	}
	valueList += ')' ;
	keyList += ")" ;
	return make_pair(keyList, valueList) ;
}

pair<string,string> getKeyValueList(const vector<string>& key, 
		const vector<string>& value, int stringNum){
	string keyList = "(" ;
	string valueList = "(" ;
	int keyLen = key.size() ;
	for (int i = 0; i < keyLen; i++) {
		if(i > 0){
			keyList += "," ;
			valueList += "," ;
		}
		keyList += key[i] ;
		if(i < stringNum)
			valueList = valueList + "'" + value[i] + "'";
		else 
			valueList += value[i] ;
	}
	valueList += ')' ;
	keyList += ")" ;
	return make_pair(keyList, valueList) ;
}

void write2Cache(const Json::Value &more, const string& username, 
		const string &fileName){
	string filePath = "./files/" ;
	filePath += username + "/cache/"+fileName ;
	FILE *filePtr = fopen(filePath.data(), "w") ;
	fwrite(more.toStyledString().data(), more.toStyledString().size(),1,filePtr);
	fclose(filePtr) ;
}
