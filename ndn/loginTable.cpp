#include <iostream>
#include <uuid/uuid.h>
#include <unistd.h>

#include "loginTable.h"
#include "common/util.h"

using namespace std;

LoginTable *LoginTable::mLoginTable = NULL ;
LoginTable *LoginTable::getInstancePtr() {
	if(mLoginTable == NULL) mLoginTable = new LoginTable() ;
	return mLoginTable ;
}

LoginTable &LoginTable::getInstance() {
	if(mLoginTable == NULL) mLoginTable = new LoginTable() ;
	return *mLoginTable ;
}

LoginTable::LoginTable(){
}
LoginTable::~LoginTable(){

}
bool LoginTable::isLogin(const string &username, const string &uuid) {
	auto it = loginTable.find(username) ;
	if(it == loginTable.end()) return false ;
	return (it->second.uuid == uuid) ;
}
string LoginTable::addLogin(const string &username) {
	string newUUID = getUUID() ;
	struct loginTableEntry newEntry ;
	newEntry.uuid = newUUID ;
	newEntry.lastActTime = getCurTimestamp();
	loginTable[username] = newEntry ;
	return newUUID ;
}
int LoginTable::updateLastTime(const string &username) {
	loginTable[username].lastActTime = getCurTimestamp() ;
	return 0 ;
}

string LoginTable::getUUID(){
	uuid_t uuid ;
	char str[40] = {0} ;
	uuid_generate(uuid) ;
	uuid_unparse(uuid,str) ;
	return str ;
}

long LoginTable::getCurTimestamp(){
	struct timeval curTime ;
	gettimeofday(&curTime,NULL) ;
	return curTime.tv_sec;
}

void LoginTable::start() {
	if(state == false ){
		pthread_t pid ;
		pthread_create(&pid , NULL, run , &loginTable) ;
		state = true ;
	}
}

void *LoginTable::run(void *argv){
	map<string, struct loginTableEntry> *loginTablePtr = 
		(map<string, struct loginTableEntry> *)argv ;
	long curTime = getCurTimestamp() ;
	while(true){
		for (auto it = loginTablePtr->begin() ; it != loginTablePtr->end() ;
				it ++) {
			if(curTime - it->second.lastActTime > 1200) loginTablePtr->erase(it) ; 
		}
		//dbg_printf("login table idle %lu\n",loginTablePtr->size()) ;
		sleep(5) ;
	}
}
