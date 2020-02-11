#ifndef _UTIL_H_
#define _UTIL_H_
#include <cstdio>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#define DEBUG

#ifdef DEBUG 
#define dbg_printf(format, ...) printf("%s:%d >> ", __FILE__, __LINE__),\
	printf(format, __VA_ARGS__)
#define dbg_perror(msg) perror(msg)
#else 
#define dbg_printf(format, ...) 
#define dbg_perror(msg) 
#endif

#define SLICE_DATA_MAX_SZ 7000

using std::string ;
using std::pair ;

class Util
{
public:
	static long getCurTimestamp() ;
	static string getFormatTime() ;
	static string getRetString(const int code, const string &errMsg,
			 const Json::Value &data ) ;
	static string getRetString(const int code, const string &errMsg);
	static string getRetString(const int code, const string &errMsg,
			const string& data);
	static string getRetStringMore(const Json::Value& data, const string &dataId, 
			const string &username) ;

	static string getFilepath(const Json::Value &root) ;
	static pair<int,int> getFileSliceNum(const char *path) ;
	static int getFileSlice(const char *path, char *buf, int sliceNo);
	static string getLongDataRet(const pair<int,Json::Value> &sqlRet , 
			const string &username , const string& fileName) ;

	static bool isSqlErr(int sqlRet) ;

private:
	/* data */
};

#endif 

	//if(!data.isNull()) {
		//dbg_printf("%s\n", data.toStyledString().data()) ;
		//FILE *fileptr = fopen("log.txt","ab+") ;
		//fprintf(fileptr , "%s\n", Util::getFormatTime().data()) ;
		//fprintf(fileptr , "%s\n", data.toStyledString().data()) ;
		//fclose(fileptr) ;
	//}
