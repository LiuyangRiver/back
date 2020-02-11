#include <iostream>
#include <sys/time.h>
#include "util.h"

using namespace std;

long Util::getCurTimestamp(){
	struct timeval curTime ;
	gettimeofday(&curTime,NULL) ;
	return curTime.tv_sec;
}
		
string Util::getFormatTime(){
    time_t tt;
    time( &tt );
    tt = tt + 8*3600;  // transform the time zone
    tm* t= gmtime( &tt );
	char buf[30] ;
	memset(buf, 0,30) ;

    sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",
           t->tm_year + 1900,
           t->tm_mon + 1,
           t->tm_mday,
           t->tm_hour,
           t->tm_min,
           t->tm_sec);
	return buf ;
}
string Util::getRetString(const int code, const string &errMsg,
		const Json::Value &data ){
	Json::Value retJson ;
	retJson["code"] = code;
	retJson["errMsg"] = errMsg;
	retJson["data"] = data ;
	return retJson.toStyledString() ;
}

string Util::getRetString(const int code, const string &errMsg){
	Json::Value retJson ;
	retJson["code"] = code;
	retJson["errMsg"] = errMsg;
	retJson["data"] = "" ;
	return retJson.toStyledString() ;
}
string Util::getRetString(const int code, const string &errMsg,
		const string &data ){
	Json::Value retJson ;
	retJson["code"] = code;
	retJson["errMsg"] = errMsg;
	retJson["data"] = data ;
	return retJson.toStyledString() ;
}

static void write2Cache(const string& dataStr, const string& username, 
		const string &dataId){
	string filepath = "./files/" ;
	filepath += username + "/cache/"+dataId ;
	dbg_printf("filepath = %s\n", filepath.data()) ;
	FILE *filePtr = fopen(filepath.data(), "w") ;
	if(filePtr == NULL ) {
		perror("file error : ") ;
		return ;
	}
	fwrite(dataStr.data(), dataStr.size(),1,filePtr);
	fclose(filePtr) ;
}

string Util::getRetStringMore(const Json::Value& data, const string &dataId, 
		const string &username){
	string dataJsonStr = "" ;
	if(!data.isNull()) dataJsonStr = data.toStyledString() ;
	Json::Value retJson ;
	int dataSize = dataJsonStr.size() ;
	dbg_printf("response dataSize = %d\n", dataSize) ;
	if(dataSize > SLICE_DATA_MAX_SZ) {
		string retFileName = "cache/" + dataId;
		Json::Value  dataInfo;
		write2Cache(dataJsonStr, username, dataId) ;
		dataInfo["dataId"] =  retFileName;
		if(dataSize % SLICE_DATA_MAX_SZ == 0) 
			dataInfo["sliceNum"] = dataSize/SLICE_DATA_MAX_SZ ;
		else 
			dataInfo["sliceNum"] = dataSize/SLICE_DATA_MAX_SZ + 1 ;
		dataInfo["dataSize"] = dataSize ;
		dataInfo["sliceSize"] = SLICE_DATA_MAX_SZ ;
		retJson["code"] = 201;
		retJson["errMsg"] = "success";
		retJson["data"] = dataInfo ;
	}else{
		retJson["code"] = 200;
		retJson["errMsg"] = "success";
		retJson["data"] = dataJsonStr;
	}
	return retJson.toStyledString() ;
}


string Util::getFilepath(const Json::Value &root){
	string filepath = "./files/" ;
	if(access(filepath.data(), F_OK) < 0) mkdir(filepath.data(), 0755) ;
	filepath = filepath+ root["username"].asString() + "/" ;
	if(access(filepath.data(), F_OK) < 0) mkdir(filepath.data(), 0755) ;
	return (filepath + root["filename"].asString()) ;
}

pair<int,int> Util::getFileSliceNum(const char *path){
	unsigned long fileSize = -1 ;
	struct stat statbuff ;
	if(stat(path, &statbuff) < 0) return make_pair(-1,-1) ;
	else fileSize = statbuff.st_size;
	int sliceNum = fileSize/SLICE_DATA_MAX_SZ ;
	if(fileSize % SLICE_DATA_MAX_SZ != 0) sliceNum ++ ;
	return make_pair(sliceNum, fileSize) ;
}
int Util::getFileSlice(const char *path, char *buf, int sliceNo){
	FILE *filePtr = fopen(path,"rb") ;
	fseek(filePtr, SLICE_DATA_MAX_SZ * sliceNo, SEEK_SET) ;
	int sliceLen = fread(buf, sizeof(char), SLICE_DATA_MAX_SZ, filePtr) ;
	dbg_printf("%d\n", sliceLen) ;
	fclose(filePtr) ;
	return sliceLen;
}
string Util::getLongDataRet(const pair<int,Json::Value> &sqlRet , 
		const string &username , const string& fileName){
	if(sqlRet.first == -1) return Util::getRetString(502,"OA database error") ;
	else if(sqlRet.first == 0) return Util::getRetString(502,"no propose");
	else if(sqlRet.first == 1) return Util::getRetString(200,"", sqlRet.second);
	else if(sqlRet.first == 2) {
		string retFileName = "cache/" + fileName ;
		Json::Value more ;
		string filePath = "files/" ;
		filePath = filePath + username + "/" + retFileName ;
		more["fileName"] =  retFileName;
		auto fileInfo = Util::getFileSliceNum(filePath.data()) ;
		more["sliceNum"] = fileInfo.first ;
		more["fileSize"] = fileInfo.second ;
		more["sliceSize"] = SLICE_DATA_MAX_SZ ;
	}
	return "" ;
}

bool Util::isSqlErr(int sqlRet){
	return (sqlRet != 0) ;
}
