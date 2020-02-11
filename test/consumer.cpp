#include <ndn-cxx/face.hpp>
#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/security/transform.hpp>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <jsoncpp/json/json.h>
using namespace::ndn ;
using namespace::std ;

const int SLICE_DATA_MAX_SZ = 6000 ;
class Consumer : noncopyable
{
	public:
	
		void mexpressInterest(string name , string param){
			static int seqNo = 0 ;
			Name interestName(name) ;
			interestName.appendSequenceNumber(seqNo++) ;
			Interest interest(interestName);
			interest.setInterestLifetime(10_s); // 2 seconds
			interest.setMustBeFresh(true);
			cout << "param size = " << param.size() << endl ;		
			interest.setApplicationParameters((const uint8_t*)param.data(), 
					param.size()) ;
			m_keyChain.sign(interest) ;
			m_face.expressInterest(interest,
					bind(&Consumer::onData, this,  _1, _2),
					bind(&Consumer::onNack, this, _1, _2),
					bind(&Consumer::onTimeout, this, _1));
			std::cout << "Sending " << interest << std::endl;
		}
		void run()
		{
			
			cout << "0 : 注册wgh1和wgh2 ;" <<endl ;
			cout<< "1 : wgh2登录 ; "<<endl ;
			cout << "2 : 获取文件信息"<<endl ; 
			cout << "3 : 下载文件 ; "<<endl ;
			cout << "4 : 发送实时信息 ; "<<endl ;
			cout << "5 : 接收实时信息 ; " << endl ;
			cout << "6 : qjm253登录 ; " <<endl ;
			cout << "7 : 查询是否有新消息" << endl ;
			cout << "8 : 通过输入用户名登录，密码默认是12345678" << endl ;
			cout << "9 : 提交审批"<< endl ;
			cout << "10 : 获取审批请求"<< endl ;
			cout << "11 : 发通知"<< endl ;
			cout << "12 : 获取通知"<< endl ;
			cout << "13 : 处理审批"<< endl ;
			cout << "14 : 获得所有用户名"<< endl ;
			cout << "15 : getNews轮询机制"<< endl ;
			cout << "16 : 聊天记录"<< endl ;
			cout << "17 : 文件操作ls / mkdir / rm"<< endl ;
			cout << "18 : 上传文件信息"<< endl ;
			cout << "19 : 上传文件"<< endl ;
			cout << "20 : getDataSlice"<< endl ;
			cin >> cmd ;
			if(cmd == 0) {
				mexpressInterest("/ndn/edu/pkusz/OA/register", getRegisterJson()) ;
				mexpressInterest("/ndn/edu/pkusz/OA/register/1", 
						getRegisterJson("wgh1", "00000000001")) ;
				mexpressInterest("/ndn/edu/pkusz/OA/register/1", 
						getRegisterJson("wgh3", "00000000004")) ;
			}
			else if(cmd ==1)
			{
				mexpressInterest("/ndn/edu/pkusz/OA/login", getLoginJson()) ;
			}
			else if(cmd == 2) {
				cout << "input uuid : " << endl ;
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/getFileInfo",
						getFileInfoJson()) ;
			}
			else if(cmd == 3) {
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ;
				cin >> uuid ;
				cout << "input file file name : " << endl ;
				cin >> filename ;
				cout << "input file slice number : " << endl ;
				cin >> sliceNum ;
				mexpressInterest("/ndn/edu/pkusz/OA/downloadFile",
						getDownloadFileJson(username, filename)) ;
			}
			else if(cmd == 4) {
				cout << "input uuid : " << endl ;
				cin >> uuid ;
				string from ;
				string to ;
				
				cout << "input from who : " << endl ;
				cin >> from ;
				cout << "input to who : " << endl ;
				cin >> to ;
				cout << "input msg number : " << endl ;
				int msgNum ;
				cin >> msgNum ;
				for (int i = 0; i < msgNum; i++) {
					mexpressInterest("/ndn/edu/pkusz/OA/sendRTMsg",
							getSendRTMsgJson(from, to)) ;
				}
			}
			else if(cmd == 5) {
				cout << "input username : " << endl ;
				cin >>  username;
				cout << "input uuid : " << endl ;
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/getRTMsg",
						getGetRTMsgJson(username)) ;
			}
			else if(cmd == 6) {
				mexpressInterest("/ndn/edu/pkusz/OA",getLoginJson("qjm253",
							"e10adc3949ba59abbe56e057f20f883e")) ;
			}else if(cmd == 7) {
				cout << "input username : " << endl ;
				string username ;
				cin >> username ;
				cout << "input uuid : " << endl ;
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/hasNews", 
						getHasNewsJson(username)) ;
			}else if(cmd == 8) {
				cout << "input username : " << endl ;
				string username ;
				cin >> username ;
				mexpressInterest("/ndn/edu/pkusz/OA/login",
						getLoginJson(username)) ;
			}else if(cmd == 9){
				string username ;
				string approver ;
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				cout << "input approver : " << endl ;
				cin >> approver ;
				for (int i = 0; i < 1; i++) {
					mexpressInterest("/ndn/edu/pkusz/OA/sendApproval",
							getSendApprovalJson(username, approver)) ;
				}
			}else if(cmd == 10){
				string username ;
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
					mexpressInterest("/ndn/edu/pkusz/OA/sendApproval",
							getGetApprovalJson()) ;
			}
			else if(cmd == 11){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				int informNum = 3 ;
				cout << "input inform num : " << endl ; 
				cin >> informNum ;
				for (int i = 0; i < informNum; i++) {
					mexpressInterest("/ndn/edu/pkusz/OA/sendApproval",
							getSendInformJson(username)) ;
				}
			}
			else if(cmd == 12){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/getInform",
						getGetInformJson(username)) ;
			}
			else if(cmd == 13){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				cout << "input approval ID : " << endl ;;
				int approvalId = 0 ;
				cin >> approvalId ;
				
				mexpressInterest("/ndn/edu/pkusz/OA/dealApproval",
						getDealApprovalJson(approvalId)) ;
			}
			else if(cmd == 14){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/getUsers",
						getUsersJson()) ;
			}
			else if(cmd == 15){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/getUsers",
						getNewsJson()) ;
			}
			else if(cmd == 16){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/getUsers",
						getGetRTMsgLogJson()) ;
			}
			else if(cmd == 17){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/getFileList",
						getFileList()) ;
			}
			else if(cmd == 18){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				mexpressInterest("/ndn/edu/pkusz/OA/uploadFileInfo",
						uploadFileInfo()) ;
			}
			else if(cmd == 19){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				string paramStr = uploadFile() ;
				cout << "======paramStr size = " << paramStr.size() << endl ;
				mexpressInterest("/ndn/edu/pkusz/OA/uploadFile",
						paramStr) ;
			}
			else if(cmd == 20) {
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ;
				cin >> uuid ;
				cout << "input data id : " << endl ;
				cin >> filename ;
				cout << "input dataSize : " << endl ;
				cin >> dataSize ; 
				if(dataSize % 7000 == 0) sliceNum = dataSize / 7000 ;
				else sliceNum = dataSize / 7000 + 1;
				this->buf = (char*)calloc(dataSize+1, sizeof(char)) ;
				mexpressInterest("/ndn/edu/pkusz/OA/getDataSlice",
						getDataSlice()) ;
			}

			m_face.processEvents();
		}

	private:

		string getDataSlice(){
			Json::Value root ;
			root["command"] = "/getDataSlice" ;
			root["username"] = username;
			root["uuid"] = uuid ;
			root["sliceNo"] = sliceNo++ ;
			root["dataId"] = filename;
			return root.toStyledString() ;
		}
		int getFileSlice(const char *path, char *buf, int sliceNo){
			FILE *filePtr = fopen(path,"rb") ;
			fseek(filePtr, SLICE_DATA_MAX_SZ * sliceNo, SEEK_SET) ;
			int sliceLen = fread(buf, sizeof(char), SLICE_DATA_MAX_SZ, filePtr) ;
			printf("%d\n", sliceLen) ;
			fclose(filePtr) ;
			return sliceLen;
		}
		string getUploadFileStr(){
			Json::Value root ;
			root["command"] = "/uploadFile" ;
			root["username"] = username;
			root["uuid"] = uuid;
			root["filename"] = filename;
			if(sliceNo >= sliceNum) return "" ;
			root["offset"] = sliceNo*SLICE_DATA_MAX_SZ;
			char buff[8000] ;
			memset(buff, 0, 8000) ;
			int jsonStrSize = root.toStyledString().size() ;
			memcpy(buff, root.toStyledString().data(), jsonStrSize) ;
			int filesliceSize = getFileSlice(localFilepath.data(), 
					buff + jsonStrSize+1 , sliceNo++) ;
			//string retStr(buff, filesliceSize + 1 + jsonStrSize) ;
			string retStr(buff, filesliceSize + 1 ) ;
			return retStr ;
		}
		string uploadFile(){
			cout << "input filename" << endl ;
			cin >> filename ;
			cout << "input local file path" << endl ;
			cin >> localFilepath ;
			auto fileinfo = getFileSliceNum(localFilepath.data()) ;
			sliceNum = fileinfo.first ;
			return getUploadFileStr() ;
		}

		pair<int,int> getFileSliceNum(const char *path){
			unsigned long fileSize = -1 ;
			struct stat statbuff ;
			if(stat(path, &statbuff) < 0) return make_pair(-1,-1) ;
			else fileSize = statbuff.st_size;
			int sliceNum = fileSize/SLICE_DATA_MAX_SZ ;
			if(fileSize % SLICE_DATA_MAX_SZ != 0) sliceNum ++ ;
			return make_pair(sliceNum, fileSize) ;
		}
		string uploadFileInfo(){
			Json::Value root ;
			root["command"] = "/uploadFileInfo" ;
			root["username"] = username;
			root["uuid"] = uuid;
			cout << "input filename" << endl ;
			string filename ;
			cin >> filename ;
			root["filename"] = filename;
			string localFilepath ;
			cout << "input local file path" << endl ;
			cin >> localFilepath ;
			auto fileinfo = getFileSliceNum(localFilepath.data()) ;
			root["fileSize"] = fileinfo.second;
			return root.toStyledString() ;
		}
		string getFileList(){
			Json::Value root ;
			root["command"] = "/fileAction" ;
			root["username"] = username;
			root["uuid"] = uuid;
			cout << "input filename" << endl ;
			string filename ;
			cin >> filename ;
			root["filename"] = filename;
			cout << "input fileCmd" << endl ;
			string fileCmd ;
			cin >> fileCmd ;
			root["fileCmd"] = fileCmd;
			return root.toStyledString() ;
		}
		string getGetRTMsgLogJson(){
			Json::Value root ;
			root["command"] = "/getRTMsgLog" ;
			root["username"] = username;
			root["uuid"] = uuid;
			cout << "input peer name: " << endl ;
			string peerName ; 
			cin >> peerName ;
			root["peer"] = peerName ;
			cout << "input time: " << endl ;
			uint64_t tmp1 ,tmp2 ;
			cin >> tmp1 ;
			root["timestamp"] = (Json::UInt64)tmp1;

			cout << "input limit: " << endl ;
			int limit ;
			cin >> limit ;
			root["limit"] = limit ;

			cout << "input direct: " << endl ;
			int direct ;
			cin >> direct ;
			root["direct"] = direct ;
			return root.toStyledString() ;
		}
		string getUsersJson(){
			Json::Value root ;
			root["command"] = "/getUsers" ;
			root["username"] = username;
			root["uuid"] = uuid;
			return root.toStyledString() ;
		}
		string getDealApprovalJson(int approvalId){
			Json::Value root ;
			root["command"] = "/dealApproval" ;
			root["username"] = username;
			root["uuid"] = uuid;
			root["approvalId"] = approvalId;
			root["approvalRst"] = 1;
			root["reason"] = "审批通过、注意出行安全";

			return root.toStyledString() ;
		}
		string getGetInformJson(string username="wgh2"){
			Json::Value root ;
			root["command"] = "/getInform" ;
			root["username"] = username;
			root["uuid"] = uuid;
			root["pageNo"] = 0;
			root["pageSize"] = 50;
			return root.toStyledString() ;
		}
		string getSendInformJson(string username = "wgh1"){
			Json::Value root ;
			root["command"] = "/sendApproval" ;
			root["username"] = username;
			root["uuid"] = uuid;
			root["approver"] ="";
			root["typeCode"] = 0;
			Json::Value detail ;
			detail["title"] = "消防演习: " ;

			std::ifstream t("file.txt");  
			std::string str((std::istreambuf_iterator<char>(t)),  
                 std::istreambuf_iterator<char>());

			detail["content"] = "今天下午，C105 消防演习" ;
			//detail["content"] = str;
			detail["startTime"] = "20191224" ;
			detail["endTime"] = "20191225" ;
			detail["attachment"] = "" ;
			root["detail"] = detail ;

			return root.toStyledString() ;

		}
		string getGetApprovalJson(){
			Json::Value root ;
			root["command"] = "/getApproval" ;
			root["username"] = username;
			root["uuid"] = uuid;
			int queryCode = 0 ;
			cout << "input queryCode " << endl ;
			cin >> queryCode ;
			root["queryCode"] = queryCode ;
			root["pageNo"] = 0 ;
			root["pageSize"] = 50 ;
			return root.toStyledString() ;
		}

		string getSendApprovalJson(string username = "wgh1", string approver="wgh2"){
			Json::Value root ;
			root["command"] = "/sendApproval" ;
			root["username"] = username;
			root["uuid"] = uuid;
			root["approver"] = approver;
			root["typeCode"] = 1;
			Json::Value detail ;
			detail["reason"] = "学术交流" ;
			detail["city"] = "深圳" ;
			detail["trafficTool"] = "飞机" ;
			detail["startTime"] = "2019-12-20" ;
			detail["endTime"] = "2019-12-24" ;
			root["detail"] = detail ;

			return root.toStyledString() ;

		}

		string getRegisterJson(string username = "wgh2", string phone = "00000000003"){
			string pubkey = "Bv0CsQcuCAR3Z2gwCANLRVkICH9+4yjVUHZaCAxjZXJ0LXJlcXVlc3QICf0AAAFv\nEUSBehQJGAECGQQANu6AFf0BJjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\nggEBANbFutCAZqFjHEa16tYvy4lBgi9/zixKWQzE9I72vaG2Imym+6KR30qkAQOK\nh0uE5RD2YsGhRK99rde+JOqIfk2/sunhp4nctOS5f4DeuOty58ekrO+Co9ZFmNBI\nmyk5i/hakjBEd/vdD7b1VadDRep5mWs3rgCinZOUfVaZ4oKqSws7ZWMvOSlRMOOi\nVzj53P4poY109S6aWmi3xKw/D76vfJapjdOB9Dg7S3rz0Rve1F/ajHCXWLt4XU3k\nUCiBn3Qmt6UD/MyDBaaUCfOvHTN6yVPtN6aOtYJVBAWt/P1ahLYhWkSm6KvrwMt0\nRzy2lw/lPriT99REItc/0wK6XwkCAwEAARZGGwEBHBcHFQgEd2doMAgDS0VZCAh/\nfuMo1VB2Wv0A/Sb9AP4PMjAxOTEyMTdUMDAzMTQw/QD/DzIwMTkxMjI3VDAwMzEz\nORf9AQCM+8WROXfi8WD9DydfLuSUxV0v3KW/Gn422/puDrkwwi1R2ogsAUc2O8pp\npuMLjWrQOTgE6ZTg80uWoDSUjjk44aMjkL2SAfp62vHRbGFx7+dckHAtF+DS6NH7\nX2vojFHOg6fksOE1M7XqLHmEEctZrFVSnQO7nB3DfRg+BqzZC/iy+8YAvTIvqA1c\nNsKYkmNITB80aM1v2H7fKnV4ZJpZnW71G5vbaPUseeklalQ2vEuBgWdd/1PL/0Ml\nI52npzqoeKAEpLp+vBwGN5ryGtWKB12nNimpMfevQAn/9BVIg59lZ7DIlE0sWmgW\nnwDxIXKebdViOejHlKwNAykxRRWw" ;
			Json::Value root ;
			root["command"] = "/register" ;
			root["username"] = username;
			root["password"] = "12345678" ;
			root["phone"] = phone;
			root["publicKey"] = pubkey;
			root["level"] = 0;
			return root.toStyledString() ;
		}

		string getLoginJson(string name="wgh2", string passwd="12345678"){
			Json::Value root ;
			root["command"] = "/login" ;
			root["username"] = name;
			root["password"] = passwd;
			return root.toStyledString() ;
		}
		string getSendRTMsgJson(string from = "wgh2", string to = "wgh1"){
			Json::Value root ;
			root["command"] = "/sendRTMsg" ;
			root["from"] = from;
			root["to"] = to;
			root["uuid"] = uuid ;
			root["msg"] = "你好,我在A栋1楼，哈,我在A栋1楼，哈,我在A栋1楼，哈,我在A栋1楼，哈,我在A栋1楼，哈,我在A栋1楼，哈,我在A栋1楼，哈,我在A栋1楼，哈,我在A栋1楼，哈,我在A栋1楼，哈哈哈哈哈哈哈哈哈哈哈" ;
			return root.toStyledString() ;
		}

		string getGetRTMsgJson(string username="wgh2"){
			Json::Value root ;
			root["command"] = "/getRTMsg" ;
			root["username"] = username;
			root["uuid"] = uuid ;
			return root.toStyledString() ;
		}

		string getFileInfoJson(){
			Json::Value root ;
			root["command"] = "/fileInfo" ;
			root["username"] = "wgh2" ;
			root["uuid"] = uuid ;
			root["filename"] = "share/avatar.png" ;
			return root.toStyledString() ;
		}
		string getDownloadFileJson(string username="wgh2",
				string filename="share/avatar.png"){
			Json::Value root ;
			root["command"] = "/downloadFile" ;
			root["username"] = username;
			root["uuid"] = uuid ;
			root["sliceNo"] = sliceNo++ ;
			root["filename"] = filename;
			return root.toStyledString() ;
		}

		string getHasNewsJson(string username="wgh2"){
			Json::Value root ;
			root["command"] = "/hasNews" ;
			root["username"] = username;
			root["uuid"] = uuid ;
			return root.toStyledString() ;
		}
		string getNewsJson(){
			Json::Value root ;
			if(isContinue == false ){
				cout << "input is continue" << endl ;
				cin >> isContinue ;
			}
			root["command"] = "/getNews" ;
			root["username"] = username;
			root["uuid"] = uuid ;
			return root.toStyledString() ;
		}

		void onData(const Interest& interest, const Data& data)
		{
			char buff[9000] ;
			memset(buff, 0 ,sizeof(buff)) ;
			std::cout << data << std::endl;
			memcpy(buff, data.getContent().value(),
					data.getContent().value_size()) ;
			if(cmd != 3)
				cout << buff << endl ;

			if(cmd == 3){
				if(sliceNo < sliceNum )
					mexpressInterest("/ndn/edu/pkusz/OA/downloadFile",
							getDownloadFileJson(username,filename)) ;

				FILE *filePtr = fopen("test.png","ab+") ;
				fseek(filePtr, 0, SEEK_END) ;
				cout <<"content size =" << data.getContent().value_size() << endl ;
				cout <<"content size =" << data.getContent().size() << endl ;
				fwrite(data.getContent().value(),data.getContent().value_size(), 
						1, filePtr) ;
				fclose(filePtr) ;
			}else if(cmd == 15 && isContinue){
				mexpressInterest("/ndn/edu/pkusz/OA/getUsers",
						getNewsJson()) ;
				sleep(1) ;
			}
			else if(cmd == 19){
				if(sliceNo < sliceNum)
					mexpressInterest("/ndn/edu/pkusz/OA/uploadFile" ,
							getUploadFileStr()) ;
			}else if(cmd == 20){
				memcpy(buf+7000*(sliceNo-1), data.getContent().value(),
						data.getContent().value_size()) ;
				if(sliceNo < sliceNum){
					mexpressInterest("/ndn/edu/pkusz/OA/getDataSlice",
							getDataSlice()) ;
				}else{
					cout << buf << endl ;
					FILE *filePtr = fopen("data","w") ;
					fwrite(buf, dataSize , 1, filePtr) ;
					fclose(filePtr) ;
				}
			}
		}

		void onNack(const Interest& interest, const lp::Nack& nack)
		{
			std::cout << "received Nack with reason " << nack.getReason()
				<< " for interest " << interest << std::endl;
		}

		void onTimeout(const Interest& interest)
		{
			std::cout << "Timeout " << interest << std::endl;
		}

	private:
		int cmd ;
		int sliceNo = 0 ;
		int sliceNum ;
		int dataSize ;
		char *buf ;
		bool isContinue = false ;

		string username = "wgh2" ;
		string filename = "wgh2.png" ;

		string localFilepath ;
		string uuid ;
		Face m_face;
		KeyChain m_keyChain ;
};


int main(int argc, char** argv)
{
	Consumer consumer;
	try {
		consumer.run();
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return 0;
}
