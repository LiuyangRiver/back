#include <iostream>
#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <stdlib.h>

#include "crypto.h"

using namespace std;

// 公钥加密    
string  Crypto::rsa_pub_encrypt(const string &clearText,
		const string &pubKeyPath)  
{  
	std::string strRet;  
	RSA* rsa ;
	FILE *filePtr ;
	filePtr = fopen(pubKeyPath.data(),"r") ;
	rsa = PEM_read_RSA_PUBKEY(filePtr, NULL, NULL, NULL);
	fclose(filePtr) ;
	if (!rsa) { return std::string(""); }
	int len = RSA_size(rsa);  
	char *encryptedText = (char *)malloc(len + 1);  
	memset(encryptedText, 0, len + 1);  
	// 加密函数  
	int ret = RSA_public_encrypt(clearText.length(), 
			(const unsigned char*)clearText.c_str(), 
			(unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);  

	cout << "ret = " << ret << endl ;
	if (ret >= 0)  
		strRet = std::string(encryptedText, ret);  

	// 释放内存  
	free(encryptedText);  
	RSA_free(rsa);

	return strRet;  
}
// 私钥解密    
string Crypto::rsa_pri_decrypt(const string &cipherText, 
		const string &priKeyPath)  
{  
	std::string strRet;  
	RSA *rsa ;  
	FILE *filePtr ;
	filePtr = fopen(priKeyPath.data(),"r") ;
	rsa = PEM_read_RSAPrivateKey(filePtr, NULL, NULL, NULL);
	fclose(filePtr) ;

	int len = RSA_size(rsa);  
	char *decryptedText = (char *)malloc(len + 1);  
	memset(decryptedText, 0, len + 1);  

	// 解密函数  
	int ret = RSA_private_decrypt(cipherText.length(), 
			(const unsigned char*)cipherText.c_str(), 
			(unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);  
	if (ret >= 0)  
		strRet = std::string(decryptedText, ret);  

	// 释放内存  
	free(decryptedText);  
	RSA_free(rsa);  

	return strRet;  
}


string Crypto::aes_encryptwithpkcs5padding(const string &plainText, 
		const string& keyStr){
	AES_KEY aes ;
	unsigned char key[AES_BLOCK_SIZE] ;		// AES_BLOCK_SIZE = 16Byte
	//unsigned char iv[AES_BLOCK_SIZE];
	int encrypBufLen ;
	if((plainText.size()) % AES_BLOCK_SIZE == 0){
		encrypBufLen = plainText.size() + AES_BLOCK_SIZE ;
	}else{
		encrypBufLen = (plainText.size()/AES_BLOCK_SIZE+1)*AES_BLOCK_SIZE;
	}
	char paddingCode = encrypBufLen - plainText.size() ;

	char *inputBuf = (char*)calloc(encrypBufLen, sizeof(char)) ;
	memcpy(inputBuf, plainText.data(), plainText.size()) ;
	memset(inputBuf+encrypBufLen-paddingCode, paddingCode, paddingCode) ;

	memcpy(key, keyStr.data(), AES_BLOCK_SIZE) ;
	//memset(iv,0,AES_BLOCK_SIZE) ;

	//int encrypRet = AES_set_encrypt_key(key, 128, &aes) ;
	AES_set_encrypt_key(key, 128, &aes) ;
	// 128是key的bit数
	char *cipherBuf = (char*)calloc(encrypBufLen, sizeof(char)) ;
	for (int i = 0; i < encrypBufLen; i+=16) {
		AES_encrypt((unsigned char*)inputBuf+i,(unsigned char*)cipherBuf+i,&aes);
	}
	string cipherStr(cipherBuf,encrypBufLen) ;
	return cipherStr ;
}

string Crypto::aes_decryptwithpkcs5padding(const string &cipherStr, 
		const string &keyStr){
	AES_KEY aes ;
	unsigned char key[AES_BLOCK_SIZE] ;		// AES_BLOCK_SIZE = 16Byte
	//unsigned char iv[AES_BLOCK_SIZE];
	memcpy(key, keyStr.data(), AES_BLOCK_SIZE) ;
	//memset(iv,0,AES_BLOCK_SIZE) ;
	//int encrypRet = AES_set_encrypt_key(key, 128, &aes) ;
	AES_set_encrypt_key(key, 128, &aes) ;
	int encrypBufLen = cipherStr.size() ;
	char *outputBuf = (char*)calloc(encrypBufLen, sizeof(char)) ;
	AES_set_decrypt_key(key, 128, &aes) ;
	for (int i = 0; i < encrypBufLen; i+=16) {
		AES_decrypt((unsigned char*)cipherStr.data()+i, 
				(unsigned char*)outputBuf+i, &aes) ;
	}
	char paddingC = outputBuf[encrypBufLen-1] ;
	//for (int i = 0; i < paddingC  ; i++) {
		//outputBuf[encrypBufLen-1-i] = 0 ;
	//}
	memset(outputBuf+encrypBufLen-paddingC, 0, paddingC) ;
	string outputStr(outputBuf) ;
	return outputStr ;
}
