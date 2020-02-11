#ifndef _NDNKEY_H_
#define _NDNKEY_H_

#include <ndn-cxx/face.hpp>
#include <iostream>

using namespace ndn ;
using namespace std ;
class NDNKey
{
public:
	NDNKey();
	~NDNKey();
	/*
	 * brief : 生成一个身份
	 * param : 
	 *		prefix 身份前缀
	 * return : 
	 *		string 自签名证书请求
	 */
	string generateKey(const string &prefix );
	/*
	 * brief : 验证证书
	 * param : 
	 *		certStr 证书字符串
	 * return : 
	 *		合法与否
	 */
	bool verifyCert(const string &certStr) ;
	/*
	 * brief : 安装证书
	 * param : 
	 *		certStr 证书字符串
	 * return : 
	 *		成功与否
	 */
	int installCert(const string &certStr) ;
	/*
	 * brief : 通过key name 得到证书 string
	 * param : 
	 *		keyName 
	 * return : 
	 *		证书string
	 */
	string getCertByKeyName(const string &keyName) ;
	/*
	 * brief : 通过key name 得到证书 string
	 * param : 
	 *		keyName 
	 * return : 
	 *		证书string
	 */
	bool delCertByKeyName(const string &keyName) ;
	/*
	 * brief : 对一个字符串进行签名 
	 * param : 
	 *		prefix 指定使用谁的私钥签名 
	 *		msg 要签名的信息
	 * return : 
	 *		返回 sign_Block的编码字符串
	 */
	string signBuf(const string &prefix, const string &msg);

	/*
	 * brief : 删除一个ID
	 * param : 
	 *		prefix 指定ID名称 
	 * return : 
	 */
	int deleteId(const string &prefix);
	/*
	 * brief : 判断一个ID的公钥在本机是否存在
	 * param : 
	 *		prefix 指定ID名称 
	 * return : 
	 *		存在与否
	 */
	bool hasId(const string &prefix) ;
private:
	/*
	 * brief : 执行一条sql命令, 无回调函数
	 * param : 
	 *		sql 要执行的命令
	 * return : 
	 *		执行成功与否
	 */
	int execSQL(const string &sql) ;
	void createCertTable();

private:
	/* data */
	KeyChain m_keyChain;
	string certTable ;
	string homePath ;
	string certDir ;
};

#endif 
