#ifndef CLIENT_H_WGH
#define CLIENT_H_WGH
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <jsoncpp/json/json.h>
#include <sys/time.h>
#include <ndn-cxx/face.hpp>
#include "ndn/ndnKey.h"
#define BUFF_SIZE 2000

using namespace std;

class DNS_Client{
		
	public:
		static DNS_Client *getInstancePtr() ;
		static DNS_Client &getInstance() ;
		~DNS_Client();
		pair<int,string> registry(const string &requestStr ) ;
		pair<int,string> destroy(string prefix); 
		pair<int,string> getAllUser();
		pair<int,string> generateIdentifier(string U_identifier, 
				string L_identifier, string prefix ,
				string Content_Hash, string AboutMe , string Other) ;
		pair<int,string> deleteIdentifier(string U_identifier, string prefix);
		pair<int,string> getUserIdentifier(string prefix);
		pair<int,string> resolve(string U_identifier);
		pair<int,string> getCert(string prefix);

		pair<int,string> phoneGetPrint(string prefix) ;
		pair<int,string> sendChkRst(string prefix) ;

		pair<int,string> sendLog(const ndn::Interest& interest) ;
		pair<int,string> sendLog(const ndn::Data& data) ;

		pair<int,string> connectServer();
		bool prefixExist(string prefix);

	private:
		DNS_Client();
		int generateQRCode(string &text , const string &path) ;
		string submitEcho(const string &msg) ;
		int getSockfd(const string &peerIp) ;
		string getTimestamp() ;
		string signJson(Json::Value& root) ;
		void loadConfig() ;
	private:
		static DNS_Client *mDNSClient ;
		string serverIP ;
		NDNKey m_ndnKey ;
		uint16_t clientPort = 0 ;
};

#endif 
