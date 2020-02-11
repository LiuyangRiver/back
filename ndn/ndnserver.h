#ifndef _NDNSERVER_H_
#define _NDNSERVER_H_
#include <ndn-cxx/face.hpp>
#include "ndn/useractHelper.h"

using namespace ndn ;
using namespace std ;

class NDNServer
{
public:
	NDNServer(const string &prefix);
	NDNServer(){};
	~NDNServer();

	void start() ;
private:
	void putData(const Name &name,const char* buf, const int bufSize);
	void onInterest(const InterestFilter& filter, const Interest& interest) ;
	void onInterestUploadFile(const InterestFilter& filter, 
			const Interest& interest) ;
	void onRegisterFailed(const Name& prefix, const std::string& reason) ;

	void dealInterest(const Interest& interest, 
			string(UserActHelper::*)(const Json::Value&)) ;

	void onIRegistry(const InterestFilter& filter, const Interest& interest) ;
	void onILogin(const InterestFilter& filter, const Interest& interest) ;
	void onIGetUsers(const InterestFilter& filter, const Interest& interest) ;
	void onISendRTMsg(const InterestFilter& filter, const Interest& interest) ;
	void onIGetRTMsg(const InterestFilter& filter, const Interest& interest) ;
	void onISendApproval(const InterestFilter& filter, const Interest& interest) ;
	void onIGetApproval(const InterestFilter& filter, const Interest& interest) ;
	void onIDealApproval(const InterestFilter& filter, const Interest& interest) ;
	void onIFileInfo(const InterestFilter& filter, const Interest& interest) ;
	void onIDownloadFile(const InterestFilter& filter, const Interest& interest);
	void onIGetDataSlice(const InterestFilter& filter, const Interest& interest);
	void onIFileAction(const InterestFilter& filter, const Interest& interest);
	void onIUploadFile(const InterestFilter& filter, const Interest& interest);
	void onIGetVCode(const InterestFilter& filter, const Interest& interest) ;
	void onIHasNews(const InterestFilter& filter, const Interest& interest) ;
	void onIGetNews(const InterestFilter& filter, const Interest& interest) ;
	void onIGetInform(const InterestFilter& filter, const Interest& interest) ;
	void onIGetRTMsgLog(const InterestFilter& filter, const Interest& interest) ;

private:
	/* data */
	string prefix ;

	map<string, void(NDNServer::*)(const InterestFilter& filter, 
			const Interest& interest)> prefixActionMap ;

	Face m_face ;
	KeyChain m_keyChain ;
	UserActHelper &mUserActHelper = UserActHelper::getInstance() ;
	DNS_Client &mDNSClient = DNS_Client::getInstance();
};

#endif 
