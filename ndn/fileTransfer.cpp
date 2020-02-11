#include <iostream>
#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/security/transform.hpp>
#include <jsoncpp/json/json.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "useractHelper.h"
#include "common/util.h"

using namespace std;

static string lsDir(const string &filepath){
	DIR *dp ;
	struct dirent *dirp ;
	if(( dp = opendir(filepath.data()) ) == NULL){
		return Util::getRetString(412,"is not a dir") ;
	}
	Json::Value retJson ;
	//struct stat s ;
	int i = 0 ;
	while((dirp = readdir(dp)) != NULL){
		Json::Value item ;
		//cout << dirp->d_name << endl ;
		if((dirp->d_name)[0] == '.') continue ;
		item["filename"] = dirp->d_name ;
		//stat(dirp->d_name , &s ) ;
		if(dirp->d_type == 4){
			item["type"] = "d" ;
		}else if(dirp->d_type == 8){
			item["type"] = "f" ;
		}else {
			item["type"] = "o" ;
		}
		if(retJson.toStyledString().size() + item.toStyledString().size() > 7000)
			break;
		retJson[i++] = item ;
	}
	closedir(dp);
	return Util::getRetString(201,"",retJson) ;
}

static string mCreateDir(const string &filepath){
	if(mkdir(filepath.data(), 0755) < 0) 
		return Util::getRetString(413,"create dir fail") ;
	return Util::getRetString(200,"success") ;
}
static string rmFile(const string &filepath){
	// 小心，先计算绝对路径，看看是否与用户主目录匹配
	
	string linuxCmd = "rm -r " + filepath ;
	FILE *fp = popen(linuxCmd.data(),"r") ;
	pclose(fp) ;
	return Util::getRetString(200,"success") ;
}
static int isInUserHome(const string &username, const string &filepath){
	string homePathStr = "./files/" + username ;
	char homePathBuf[1000] ;
	memset(homePathBuf,0,sizeof(homePathBuf)) ;
	realpath(homePathStr.data(), homePathBuf) ;
	char filepathBuf[1000] ;
	memset(filepathBuf,0,sizeof(filepathBuf)) ;
	realpath(filepath.data(), filepathBuf) ;
	homePathStr = homePathBuf ;
	string filepathStr = filepathBuf ;
	dbg_printf("homePathStr : %s \n", homePathStr.data()) ;
	dbg_printf("filepathStr : %s \n", filepathStr.data()) ;
	if(filepathStr == homePathStr) return 2 ;
	else if(filepathStr.find(homePathStr) == 0) return 1 ;
	return 0 ;

}

string UserActHelper::fileAction(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid","fileCmd", "filename"};
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	string username = root["username"].asString() ;
	string filepath = "./files/" + username + root["filename"].asString() ;
	int isInHome = isInUserHome(username, filepath) ;
	if(!isInHome) return Util::getRetString(413,"file operation error") ;
	string cmd = root["fileCmd"].asString() ;
	if(cmd == "ls"){
		if(access(filepath.data(), F_OK) < 0) 
			return Util::getRetString(412,"file does not exist") ;
		return lsDir(filepath) ;
	}else if(cmd == "rm"){
		if(access(filepath.data(), F_OK) < 0) 
			return Util::getRetString(412,"file does not exist") ;
		if(isInHome == 2) return Util::getRetString(413,"file operation error") ;
		return rmFile(filepath) ;
	}else if(cmd == "mkdir"){
		if(isInHome == 2) return Util::getRetString(413,"file operation error") ;
		return mCreateDir(filepath) ;
	}
	return Util::getRetString(413,"file operation error") ;
}
string UserActHelper::fileInfo(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid", "filename"};
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	string filepath = Util::getFilepath(root) ;
	if(access(filepath.data(), F_OK) < 0) 
		return Util::getRetString(412,"file does not exist") ;

	auto fileSZInfo = Util::getFileSliceNum(filepath.data()) ;
	if(fileSZInfo.first <= 0) 
		return Util::getRetString(412,"file does not exist") ;

	Json::Value fileInfoJson ;
	fileInfoJson["sliceNum"] = fileSZInfo.first ;
	fileInfoJson["fileSize"] = fileSZInfo.second ;
	fileInfoJson["sliceSize"] = SLICE_DATA_MAX_SZ ;
	return Util::getRetString(200,"success", fileInfoJson) ;
}

string UserActHelper::downloadFile(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid", "filename", "sliceNo"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	string filepath = Util::getFilepath(root) ;
	if(access(filepath.data(), F_OK) < 0) 
		return Util::getRetString(412,"file does not exist") ;
	int sliceNum = Util::getFileSliceNum(filepath.data()).first ;
	int sliceNo = root["sliceNo"].asInt() ;
	if(sliceNo >= sliceNum) return "" ;
	char fileSliceBuf[8000] ;
	int fileSliceLen = Util::getFileSlice(filepath.data(), fileSliceBuf, sliceNo) ;
	string fileSliceStr(fileSliceBuf, fileSliceBuf+fileSliceLen) ;
	return fileSliceStr ;
}

string UserActHelper::getDataSlice(const Json::Value &root) {
	vector<string> keyname = {"username", "uuid", "dataId", "sliceNo"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	string filepath = "./files/" + root["username"].asString() + "/" 
		+ root["dataId"].asString() ;

	if(access(filepath.data(), F_OK) < 0) 
		return Util::getRetString(412,"data does not exist") ;
	int sliceNum = Util::getFileSliceNum(filepath.data()).first ;
	int sliceNo = root["sliceNo"].asInt() ;
	if(sliceNo >= sliceNum) return "" ;
	char fileSliceBuf[8000] ;
	int fileSliceLen = Util::getFileSlice(filepath.data(), fileSliceBuf, sliceNo) ;
	string fileSliceStr(fileSliceBuf, fileSliceBuf+fileSliceLen) ;
	return fileSliceStr ;
}

string UserActHelper::uploadFile(const Json::Value &root, const char* buf,
		const int bufSize) {
	vector<string> keyname = {"username", "uuid", "filename", "offset"} ;
	auto chkRet = chkIntAndLogin(keyname,root) ;
	if(!chkRet.first) return chkRet.second ;
	string filepath = Util::getFilepath(root) ;
	string username = root["username"].asString() ;
	if(!isInUserHome(username, filepath))
		return Util::getRetString(413,"file operation error") ;

	dbg_printf("bufSize = %d\n", bufSize) ;

	int filefd ;
	if(access(filepath.data(), F_OK) < 0) filefd = creat(filepath.data(), 0644) ;
	else filefd = open(filepath.data(), O_WRONLY) ;
	if(filefd < 0) return Util::getRetString(413,"create file failed") ;
	int offset = root["offset"].asInt() ;
	if(offset <0 || offset > 300000000) 
		Util::getRetString(413,"file operation error") ;
	lseek(filefd, offset, SEEK_SET) ;
	int ret = write(filefd, buf, bufSize) ;
	close(filefd) ;
	if(ret < 0) Util::getRetString(413,"file operation error") ;
	return Util::getRetString(200,"upload success") ;
}

