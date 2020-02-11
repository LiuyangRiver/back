#include <unistd.h>
#include <fstream>
#include <cryptopp/base64.h>
#include <cctype>
#include <regex>
#include "dns_client.h"
#include "common/util.h"

DNS_Client * DNS_Client::mDNSClient ;
DNS_Client *DNS_Client::getInstancePtr(){
	if(mDNSClient == NULL) mDNSClient = new DNS_Client() ;
	return mDNSClient ;
}

DNS_Client &DNS_Client::getInstance() {
	if(mDNSClient == NULL) mDNSClient = new DNS_Client() ;
	return *mDNSClient ;
}
void DNS_Client::loadConfig(){
	string file_path = "./config.json" ;
	Json::Reader reader ;
	Json::Value root ;
	std::ifstream in(file_path.data(),ios::binary);
	if(!in.is_open()){
		cout << "can not open file " << file_path << endl ;
		exit(1);
	}
	reader.parse(in,root);
	in.close();
	if(!root.isMember("blockchainServers")){
		cout << "please configure blockserver first" << endl ;
		exit(1) ;
	}
	int serverNum = root["blockchainServers"].size() ;
	for(int i = 0 ;i < serverNum; i++){
		string ip = root["blockchainServers"][i].asString() ;
		int sockfd = getSockfd(ip) ;
		if(sockfd > 0) {
			dbg_printf("blockchainServers %s will be used\n",ip.data()) ;
			this->serverIP = ip ;
			close(sockfd) ;
			return ;
		}
	}
	cout << "do not find available blockchainServers" << endl ;
	exit(1) ;
}
DNS_Client::DNS_Client(){
	loadConfig() ;	
}
DNS_Client::~DNS_Client(){
}

int DNS_Client::getSockfd(const string &peerIp) {
	struct sockaddr_in server_addr ;
	memset(&server_addr,0,sizeof(server_addr)) ;
	server_addr.sin_family = PF_INET ;
	server_addr.sin_addr.s_addr = inet_addr(peerIp.data());
	server_addr.sin_port = htons(8010) ;
	struct sockaddr_in clientAddr ;
	memset(&clientAddr,0,sizeof(clientAddr)) ;
	clientAddr.sin_family = PF_INET ;
	clientAddr.sin_port = htons(this->clientPort) ;
	socklen_t clientAddrLen = sizeof(clientAddr) ;
	int sockfd = socket(PF_INET, SOCK_STREAM, 0 ) ;
	bool bindStatus = true ;
	int on = 1;
	setsockopt(sockfd , SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) ;

	if(this->clientPort == 0){
		bindStatus = false ;
	}else if(bind(sockfd, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) < 0){
		perror("bind error : ") ;
		memset(&clientAddr,0,sizeof(clientAddr)) ;
		bindStatus = false ;
	}
	int ret = connect(sockfd , (struct sockaddr *)&server_addr , 
			sizeof(server_addr));
	if(ret < 0 ){
		perror("connect to server error : ") ;
		close(sockfd) ;
		return -1 ;
	}
	if(!bindStatus){
		memset(&clientAddr,0,sizeof(clientAddr)) ;
		getsockname(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen) ;
		this->clientPort = ntohs(clientAddr.sin_port) ;
	}
	getsockname(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen) ;
	dbg_printf("use port %hu\n",ntohs(clientAddr.sin_port)) ;
	return sockfd ;
}


static bool chkIPformat(string &ip){
	regex pattern("([0-9]{1,3}\\.){3}([0-9]{1,3})") ;
	return std::regex_match(ip, pattern) ;
}

pair<int, string> DNS_Client::connectServer(){
	FILE *file = fopen("./resolv.conf", "r") ;
	if(file == NULL){
		perror("open file error : ") ;
		return make_pair(-1, 
				"can not find the resolve file for block chain server") ;
	}
	char buf[100] ;
	memset(buf,0,sizeof(buf)) ;
	int sockfd = -1 ;
	while(fscanf(file,"%s",buf) != EOF ){
		string bufstr = buf ;
		if(isdigit(buf[0])){
			if(!chkIPformat(bufstr)){
				//cout << "IP : " << bufstr << " format error" << endl ;
			}else if((sockfd = getSockfd(bufstr)) > 0){
				this->serverIP = bufstr ;
				cout << "server : " << bufstr << " will be used" << endl ;
				fclose(file) ;
				close(sockfd) ;
				return make_pair(0, this->serverIP) ;
			}else{
				cout << "server : " << bufstr << " no open" << endl ;
			}
		}
		memset(buf,0,sizeof(buf)) ;
	}
	fclose(file) ;
	return make_pair(-1, "no server found") ;
}


string DNS_Client::submitEcho(const string &msg){
	string result = "" ;
	int buffSize = msg.size()+5 ;
	char *buff = (char*)malloc(buffSize) ;
	int len = msg.length() ;
	memcpy(buff,&len, sizeof(int)) ;
	memcpy(buff+sizeof(int), msg.data(), msg.length()) ;
	int sockfd = getSockfd(this->serverIP) ;
	int wLen = write(sockfd, buff , msg.length() + 4 ) ;
	if(wLen <= 0){
		perror("send to server error : ") ;
		close(sockfd) ;
		free(buff) ;
		return "" ;
	}
	int recvLen = read(sockfd, buff, buffSize) ;
	memcpy(&len , buff, sizeof(int)) ;
	if(len+5 > buffSize){
		buffSize = len+5 ;
		buff = (char*)realloc(buff , buffSize) ;
		//dbg_printf("dns tcp realloc buff =  %p\n", buff) ;
	}
	while(recvLen < len + 4){
		int rLen = read(sockfd, buff+recvLen, buffSize - recvLen) ;
		if(rLen <= 0){
			perror("recv from server error : ") ;
			close(sockfd) ;
			free(buff) ;
			return "" ;
		}
		recvLen += rLen ;
	}
	close(sockfd) ;
	buff[recvLen] = 0 ;
	result = buff+4 ;
	free(buff) ;
	return  result;
}

string DNS_Client::getTimestamp() {
	struct timeval curTime ;
	gettimeofday(&curTime,NULL) ;
	return to_string(curTime.tv_sec) ;
}


pair<int,string> DNS_Client::registry(const string &requestStr ) {
	//auto pairRet = connectServer() ;
	//if(pairRet.first < 0 )
		//return pairRet;
	string retStr = submitEcho(requestStr) ;
	if(retStr == "") return make_pair(-1,"IO with block chain error") ;
	Json::Value root ;
	Json::Reader reader ;
	reader.parse(retStr, root) ;
	if(root["StatusCode"].asInt() >= 300) {
		return make_pair(-1, "BlockChain says: request format error") ;
	}
	return make_pair(0,retStr) ;
}

pair<int, string> DNS_Client::destroy(string prefix){
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	if(!m_ndnKey.hasId(prefix)){
		return make_pair(-1, "can not find local private key") ;
	}
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "Destroy" ;
	root["Prefix"	] =  prefix ;
	root["Timestamp"] =  getTimestamp() ;
	string sig = signJson(root) ;
	root["Sig"] = sig ;
	m_ndnKey.deleteId(prefix) ;
	m_ndnKey.delCertByKeyName(prefix) ;
	return make_pair(0,submitEcho(root.toStyledString()) );
}

pair<int, string> DNS_Client::getAllUser(){
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "getUser" ;
	root["QueryCode"] =  0;
	return make_pair(0,submitEcho(root.toStyledString())) ;
}

pair<int, string> DNS_Client::generateIdentifier(string U_identifier, 
		string L_identifier, string prefix, string Content_Hash, 
		string AboutMe , string Other){
	
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"			] = "user-act";
	root["Command"		] = "Generate";
	root["U_identifier"	] = U_identifier;
	root["L_identifier"	] = L_identifier;
	root["Prefix"		] = prefix;
	root["Content_Hash"	] = Content_Hash;
	root["Timestamp"	] = getTimestamp();
	root["AboutMe"		] =	AboutMe;
	root["Other"		] = Other;
	string sig = signJson(root) ;
	root["Sig"] = sig ;

	string retStr = submitEcho(root.toStyledString() );
	
	Json::Value retJson ;
	Json::Reader reader ;
	reader.parse(retStr , retJson) ;
	if(retJson["StatusCode"].asInt() > 299){
		//loginWeb(prefix) ;
		return make_pair(-1,"you did not login yet") ;
	}

	return make_pair(0, retStr);
}

pair<int, string> DNS_Client::deleteIdentifier(string U_identifier, 
		string prefix){
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "Delete" ;
	root["U_identifier"] =  U_identifier;
	root["Prefix"] =  prefix;
	string sig = signJson(root) ;
	root["Sig"] = sig ;

	return make_pair(0,submitEcho(root.toStyledString()) );
}

pair<int, string> DNS_Client::getUserIdentifier(string prefix){
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "Query" ;
	root["QueryCode"] =  1;
	root["Prefix"] =  prefix;
	return make_pair(0,submitEcho(root.toStyledString()) );
}

bool DNS_Client::prefixExist(string prefix){
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "Query" ;
	root["QueryCode"] =  4;
	root["Prefix"] =  prefix;
	string ret = submitEcho(root.toStyledString()) ;
	Json::Reader reader ;
	reader.parse(ret, root) ;
	if(root["Message"] == "true") return true ;
	return false;
}

pair<int, string> DNS_Client::resolve(string U_identifier){
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "Resolve" ;
	root["QueryCode"] =  2;
	root["U_identifier"] =  U_identifier;
	return make_pair(0,submitEcho(root.toStyledString()) );
}
pair<int, string> DNS_Client::getCert(string prefix){
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "GetPubkey" ;
	root["Prefix"] =  prefix;

	string retStr = submitEcho(root.toStyledString() );
	Json::Reader reader ;
	reader.parse(retStr, root) ;
	
	m_ndnKey.installCert(root["Message"].asString()) ;

	return make_pair(0, retStr);
}

pair<int, string> DNS_Client::phoneGetPrint(string prefix) {
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "Query" ;
	root["QueryCode"] =  3;
	root["Prefix"] =  prefix;
	root["Timestamp"] = getTimestamp() ;
	root["Sig"] =  "xxx";    
	return make_pair(0,submitEcho(root.toStyledString()) );
}
pair<int, string> DNS_Client::sendChkRst(string prefix) {
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"		] =  "user-act";
	root["Command"	] =  "Login" ;
	root["Prefix"] = prefix;
	root["ChkRst"] = 1 ;
	root["Timestamp"] = getTimestamp() ;
	root["Sig"] =  "xxx";    
	return make_pair(0,submitEcho(root.toStyledString()) );
}


pair<int,string> DNS_Client::sendLog(const ndn::Interest& interest) {
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	
	string prefix = "" ;
	try{
		SignatureInfo signInfo(interest.getName()[signed_interest::POS_SIG_INFO]
				.blockFromValue()) ;
		prefix = signInfo.getKeyLocator().getName().toUri();
	}catch(...){
		return make_pair(-1, "interest format error") ;
	}
	Json::Value root ;
	root["Type"		] =  "network";
	root["Command"	] =  "Log" ;
	root["Prefix"] = prefix ;
	root["Sig"] = "xxx";
	root["Action"] = interest.getName().toUri() ;
	root["Timestamp"] = getTimestamp() ;
	return make_pair(0,submitEcho(root.toStyledString()) );
}
pair<int,string> DNS_Client::sendLog(const ndn::Data& data) {
	//if(connectServer().first < 0 )
		//return make_pair(-1, "can not find available server") ;
	Json::Value root ;
	root["Type"		] =  "network";
	root["Command"	] =  "Log" ;
	root["Prefix"] = data.getSignature().getKeyLocator().getName().toUri();
	root["Sig"] = "xxx";
	root["Action"] = data.getName().toUri() + " >> data";
	root["Timestamp"] = getTimestamp() ;
	return make_pair(0,submitEcho(root.toStyledString()) );
}

//static string getBase64(const string &msg){
	//string result ;
	//CryptoPP::StringSource((const byte*)msg.data(), msg.length(), true, 
			//new CryptoPP::Base64Encoder(new CryptoPP::StringSink(result), false));
	//return result ;
//}

string DNS_Client::signJson(Json::Value& root) {
	if( root.isMember("Sig") ) root.removeMember("Sig") ;
	string sig = m_ndnKey.signBuf(root["Prefix"].asString(),
			root.toStyledString()) ;
	return sig ;
}

