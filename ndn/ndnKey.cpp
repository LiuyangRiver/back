#include <iostream>
#include <fstream>
#include <sstream>
#include <ndn-cxx/util/io.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
#include <unistd.h>
#include <sys/stat.h> 
#include <sqlite3.h>

#include "ndnKey.h"

using namespace std;

static bool pathExist(const string &path){
	return ( access(path.data() , 0) == 0 ) ;
}

static void mkdirRecur(const string &path){
	int idx = path.find_last_of('/') ;
	string fatherDir = path.substr(0,idx) ;
	if(!pathExist(fatherDir)) mkdirRecur(fatherDir) ;
	mkdir(path.data(), 0755) ;
}

//static void string2file(const string &str, const string& path){
	//ofstream ofs ;
	//ofs.open(path.data()) ;
	//ofs << str ;
	//ofs.close() ;
//}

int NDNKey::execSQL(const string &sql){
	sqlite3 *sqliteDB  ;
	int execRet = sqlite3_open(certTable.data(), &sqliteDB) ;
	if(execRet != SQLITE_OK){
		cout << "open sqlite database fail" << endl ;
	}
	execRet = sqlite3_exec(sqliteDB, sql.data(), 0, 0, NULL) ;
	sqlite3_close(sqliteDB) ;
	if(execRet == SQLITE_OK){
		cout << "OK" << endl ;
		return 1 ;
	}else {
		cout << "failed" << endl ;
		return 0 ;
	}
	return 0 ;
}

void NDNKey::createCertTable(){
	string sql = "create table cert (name text primary key not NULL, value text not NULL)" ;
	execSQL(sql) ;
}

NDNKey::NDNKey(){
	this->homePath = getenv("HOME") ;
	this->certTable = homePath + "/.ndn/certs.db" ;
	this->certDir = homePath + "/.ndn" ;
}
NDNKey::~NDNKey(){

}

string NDNKey::generateKey(const string &prefix ){
	security::Identity identity = m_keyChain.createIdentity(Name(prefix.data())) ;
	security::Key key = identity.getDefaultKey() ;
	security::v2::Certificate cert ;
	Name certName = key.getName() ;
	certName.append("cert-request").appendVersion() ;
	cert.setName(certName) ;

	cert.setContentType(tlv::ContentType_Key) ;
	cert.setFreshnessPeriod(10_h) ;

	cert.setContent(key.getPublicKey().data(), key.getPublicKey().size()) ;

	SignatureInfo signatureInfo ;
	auto now = time::system_clock::now() ;
	signatureInfo.setValidityPeriod(security::ValidityPeriod(now, now + 10_days));

	m_keyChain.sign(cert, security::SigningInfo(key)
			.setSignatureInfo(signatureInfo)) ;
	std::stringstream ss ;
	io::save(cert,ss) ;	
	return ss.str() ;
}

bool NDNKey::verifyCert(const string &certStr) {
	std::stringstream ss ;
	ss << certStr ;
	security::v2::Certificate cert = *(io::load<security::v2::Certificate>(ss)) ; 
	string keyPath = cert.getSignature().getSignatureInfo().getKeyLocator()
		.getName().toUri() ;
	
	std::stringstream ss1 ;
	ss1 << getCertByKeyName(keyPath) ;
	if(ss1.str().size() == 0) {
		cout << "can not find cert" << endl ;
		return false ;
	}
	security::v2::Certificate signerCert = 
		*(io::load<security::v2::Certificate>(ss1));

	bool ret = ndn::security::verifySignature(cert , signerCert) ;
	return ret ;
}
int NDNKey::installCert(const string &certStr) {
	std::stringstream ss ;
	ss << certStr ;
	security::v2::Certificate cert = *(io::load<security::v2::Certificate>(ss)) ; 

	if(!pathExist(this->certDir)){
		mkdirRecur(certDir)	;
		createCertTable() ;
	}else if(!pathExist(this->certTable)){
		createCertTable() ;
	}

	string sql = "insert into cert (name, value) values ('" ;
	sql = sql + cert.getName().toUri() + "','"+ certStr +"')" ;
	int execRet = execSQL(sql) ;
	if(execRet){
		return 1 ;
	}else{
		return 0 ;
	}
	return 0 ;
}

static int sqliteSelectCallback(void *data,int args_num,char **argv,char **argc){
	strcpy((char*)data, argv[0]) ;
	return 0 ;
}
string NDNKey::getCertByKeyName(const string &keyName){
	string sql = "select value from cert where name like '" ;
	sql = sql + keyName +"%'" ;
	char certBuf[2000] ;
	memset(certBuf, 0, 2000) ;
	sqlite3 *sqliteDB  ;

	int execRet = sqlite3_open(certTable.data(), &sqliteDB) ;
	if(execRet != SQLITE_OK){ cout << "open sqlite database fail" << endl ; }
	execRet = sqlite3_exec(sqliteDB, sql.data(), sqliteSelectCallback, certBuf,
			NULL) ;
	sqlite3_close(sqliteDB) ;
	return certBuf ;
}
bool NDNKey::delCertByKeyName(const string &keyName) {
	string sql = "delete from cert where name like '" ;
	sql = sql + keyName +"%'" ;
	return execSQL(sql) ;
}

string NDNKey::signBuf(const string &prefix, const string &msg){
	security::SigningInfo signinfo = 
		security::signingByIdentity(Name(prefix.data())) ;
	Block sign = m_keyChain.sign((const uint8_t*)msg.data(), msg.size(),
			signinfo) ;

	sign.encode() ;		// 必须要编码一下，不知为何
	std::stringstream ss ;
	io::saveBlock(sign, ss) ;
	return ss.str() ;
}

bool NDNKey::hasId(const string &prefix) {
	try{
		const ndn::security::Identity &id = m_keyChain.getPib()
			.getIdentity(Name(prefix.data())) ;
	}catch(std::exception &e){
		return false ;
	}
	return true ;
}

int NDNKey::deleteId(const string &prefix){
	if(!hasId(prefix)) return -1 ;
	m_keyChain.deleteIdentity( 
			m_keyChain.getPib().getIdentity(Name(prefix.data()))  ) ;
	return 0 ;
}
