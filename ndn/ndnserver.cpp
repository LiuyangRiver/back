#include <iostream>
#include <pthread.h>
#include "ndnserver.h"
#include "common/util.h"

using namespace std;

NDNServer::NDNServer(const string &prefix):
	prefix(prefix)
{
	//mUserActHelperPtr = UserActHelper::getInstancePtr() ;
	prefixActionMap["/register"] = &NDNServer::onIRegistry ;
	prefixActionMap["/login"] = &NDNServer::onILogin ;
	prefixActionMap["/getUsers"] = &NDNServer::onIGetUsers ;
	prefixActionMap["/sendRTMsg"] = &NDNServer::onISendRTMsg ;
	prefixActionMap["/getRTMsg"] = &NDNServer::onIGetRTMsg ;
	prefixActionMap["/sendApproval"] = &NDNServer::onISendApproval ;
	prefixActionMap["/getApproval"] = &NDNServer::onIGetApproval ;
	prefixActionMap["/dealApproval"] = &NDNServer::onIDealApproval ;
	prefixActionMap["/getVCode"] = &NDNServer::onIGetVCode ;

	prefixActionMap["/fileInfo"] = &NDNServer::onIFileInfo ;
	prefixActionMap["/downloadFile"] = &NDNServer::onIDownloadFile;
	prefixActionMap["/getDataSlice"] = &NDNServer::onIGetDataSlice;
	prefixActionMap["/fileAction"] = &NDNServer::onIFileAction;
	prefixActionMap["/uploadFile"] = &NDNServer::onIUploadFile;

	prefixActionMap["/getVCode"] = &NDNServer::onIGetVCode ;
	prefixActionMap["/hasNews"] = &NDNServer::onIHasNews ;
	prefixActionMap["/getNews"] = &NDNServer::onIGetNews ;
	prefixActionMap["/getInform"] = &NDNServer::onIGetInform ;
	prefixActionMap["/getRTMsgLog"] = &NDNServer::onIGetRTMsgLog ;
}

NDNServer::~NDNServer(){
}

void NDNServer::start() {
	cout << "prefix : " << prefix << endl ;
	m_face.setInterestFilter(prefix,
			bind(&NDNServer::onInterest, this, _1,_2) ,
			RegisterPrefixSuccessCallback() ,
			bind(&NDNServer::onRegisterFailed, this, _1, _2) );

	m_face.setInterestFilter(prefix+"/uploadFile",
			bind(&NDNServer::onInterestUploadFile, this, _1,_2) ,
			RegisterPrefixSuccessCallback() ,
			bind(&NDNServer::onRegisterFailed, this, _1, _2) );
	m_face.processEvents();
}


void NDNServer::putData(const Name &name,const char* buf, const int bufSize){
	Data data ;
	data.setName(name) ;
	data.setFreshnessPeriod(0_s) ;
	data.setContent(reinterpret_cast<const uint8_t*>(buf), bufSize ) ;
	m_keyChain.sign(data) ;
	//dbg_printf("data content size = %lu\n", data.getContent().value_size()) ;
	m_face.put(data) ;
}

void NDNServer::onInterestUploadFile(const InterestFilter& filter, 
		const Interest& interest) {
	//dbg_printf("I: %s\n",interest.getName().toUri().data()) ;
	string reply = "" ;
	char fileBuf[8000] ;
	auto logRet = mDNSClient.sendLog(interest) ;
	if(logRet.first < 0 ){
		reply = Util::getRetString(411,"interest format error") ;
	}
	else if(interest.hasApplicationParameters()){
		string jsonStr = (char*)(interest.getApplicationParameters().value()) ;
		int fileSliceSize = 
			interest.getApplicationParameters().value_size()-jsonStr.size()-1 ;
		if(fileSliceSize <= 0 ) return ;
		memcpy(fileBuf, 
				interest.getApplicationParameters().value() + jsonStr.size()+1, 
				fileSliceSize);
		reply = mUserActHelper.dealReq(jsonStr, fileBuf, fileSliceSize) ;
	}
	if(reply.size() >0){
		putData(interest.getName(), reply.data(), reply.size()) ;
	}

}
void NDNServer::onInterest(const InterestFilter& filter, 
		const Interest& interest) {
	//dbg_printf("I: %s\n",interest.getName().toUri().data()) ;
	auto logRet = mDNSClient.sendLog(interest) ;
	string reply = "" ;
	if(logRet.first < 0 ){
		reply = Util::getRetString(411,"interest format error") ;
	}
	else if(interest.hasApplicationParameters())
	{
		string intParaStr((char*)interest.getApplicationParameters().value(), 
				interest.getApplicationParameters().value_size()) ;
		reply = mUserActHelper.dealReq(intParaStr) ;
	}else{
		cout << "interest has not parameter: " << interest << endl ;
	}
	if(reply.size() > 0){
		putData(interest.getName(), reply.data(), reply.size()) ;
	}else{
		cout << "data empty" << endl ;
	}
}
void NDNServer::onRegisterFailed(const Name& prefix, const std::string& reason) {
	cout << "register prefix : " << prefix << " error" << endl ;
}

void NDNServer::dealInterest(const Interest& interest, 
		string(UserActHelper::*)(const Json::Value&)){
	
}

void NDNServer::onIRegistry(const InterestFilter& filter, 
		const Interest& interest) {
	//dealInterest(interest, (mUserActHelper.registry)) ;
}
void NDNServer::onILogin(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIGetUsers(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onISendRTMsg(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIGetRTMsg(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onISendApproval(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIGetApproval(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIDealApproval(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIGetVCode(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIFileInfo(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIDownloadFile(const InterestFilter& filter, 
		const Interest& interest){

}
void NDNServer::onIGetDataSlice(const InterestFilter& filter, 
		const Interest& interest){

}
void NDNServer::onIFileAction(const InterestFilter& filter, 
		const Interest& interest){

}
void NDNServer::onIUploadFile(const InterestFilter& filter, 
		const Interest& interest){

}
void NDNServer::onIHasNews(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIGetNews(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIGetInform(const InterestFilter& filter, 
		const Interest& interest) {

}
void NDNServer::onIGetRTMsgLog(const InterestFilter& filter, 
		const Interest& interest) {

}
