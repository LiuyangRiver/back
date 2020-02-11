#ifndef _LOGINTABLE_H_
#define _LOGINTABLE_H_
#include <map>
#include <sys/time.h>

using std::map ;
using std::string ;
struct loginTableEntry{
	string uuid ;
	long lastActTime ;
	string aesKey ;
};
class LoginTable
{
public:
	static LoginTable *getInstancePtr() ;
	static LoginTable &getInstance() ;
	~LoginTable();
	bool isLogin(const string &username, const string& uuid) ;
	/*
	 * brief : 登录表中添加用户
	 * ret : 为用户生成的UUID
	 */
	string addLogin(const string &username) ;
	int updateLastTime(const string &username) ;

	void start() ;

private:
	LoginTable();
	static void* run(void * argv) ;
	string getUUID() ;
	static long getCurTimestamp() ;
private :
	/* data */
	bool state = false ;
	static LoginTable *mLoginTable ;
	map<string,struct loginTableEntry> loginTable ;
};

#endif 
