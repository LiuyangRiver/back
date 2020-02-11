#ifndef _USERACTHELPER_H_
#define _USERACTHELPER_H_
#include <iostream> 
#include "blockchain/dns_client.h"
#include "mysql/sqlfunc.h"
#include "ndn/loginTable.h"


using std::string ;
class UserActHelper
{
public:
	~UserActHelper();
	static UserActHelper * getInstancePtr() ;
	static UserActHelper & getInstance() ;
	static void destroyInstance(){ if(!mUserActHelper) delete mUserActHelper ;} ;
	
	string dealReq(const string &requestStr) ;
	string dealReq(const string &requestStr, const char* buf, const int bufSize) ;
	pair<bool, string> sendLog() ;

//private:
	string registry(const Json::Value &root) ;		// 注册
	string doRegistry(const Json::Value &root, const Json::Value &user) ;
	string login(const Json::Value &root) ;			// 登录
	string getVCode(const Json::Value &root) ;		// 获得手机验证码
	string getUserInfo(const Json::Value &root) ;		// 获得用户信息
	string updateUserInfo(const Json::Value &root) ;		// 更改用户信息

	// ===================================实时通信相关
	string getUsers(const Json::Value &root) ;	// 获得所有用户名
	string sendRTMsg(const Json::Value &root) ;	// 发送实时信息
	string getRTMsg(const Json::Value &root) ;		// 获取实时信息
	string getRTMsgLog(const Json::Value &root) ;	// 聊天记录


	// ===================================审批相关
	string sendApproval(const Json::Value &root) ;	// 提交审批
	string getApproval(const Json::Value &root) ;	// 获取审批
	string getApprovalById(const Json::Value &root) ;  // 通过id获取审批
	string dealApproval(const Json::Value &root) ;	// 处理审批
	string getInform(const Json::Value &root) ;		// 获取通知

	// ===================================文件相关
	string uploadFile(const Json::Value &root, const char* buf, 
			const int bufSize) ;	// 上传文件
	//string uploadFileInfo(const Json::Value &root) ;	// 上传文件信息
	string fileInfo(const Json::Value &root) ;		// 获得文件信息
	string downloadFile(const Json::Value &root) ;	// 下载文件分片
	string fileAction(const Json::Value &root) ;	// 获取某目录文件列表
	string getDataSlice(const Json::Value &root) ;	// 获取数据


	string hasNews(const Json::Value &root) ;		// 查询最新消息
	string getNews(const Json::Value &root) ;


	/*
	 * brief : 检查json字段是否完整，用户是否登录
	 * param : keyname 必须的字段键值数组，其中keyname[0]为用户名 
	 *			root  json对象
	 * ret : 错误返回false ;
	 *
	 */ 
	pair<bool,string> chkIntAndLogin(const vector<string> &keyname, 
			const Json::Value &root );
	bool chkJsonKeyIntegrity(const vector<string> &keyname,
			const Json::Value &root);
	UserActHelper();
private:
	static UserActHelper * mUserActHelper;
	map<string, string(UserActHelper::*)(const Json::Value&)> actionMap ;

	SqlFunc mSqlFunc ;
	DNS_Client &mDnsClient = DNS_Client::getInstance();
	LoginTable *mLoginTablePtr ;
};
#endif 
