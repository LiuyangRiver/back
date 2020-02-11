#include <ndn-cxx/face.hpp>
#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/security/transform.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
using namespace::ndn ;
using namespace::std ;

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
			cout << "17 : 向qjm253发实时信息"<< endl ;
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
				for (int i = 0; i < 1; i++) {
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
				for (int i = 0; i < 20; i++) {
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
							getGetApprovalJson(username)) ;
			}
			else if(cmd == 11){
				cout << "input username : " << endl ;
				cin >> username ;
				cout << "input uuid : " << endl ; 
				cin >> uuid ;
				for (int i = 0; i < 40; i++) {
					mexpressInterest("/ndn/edu/pkusz/OA/sendApproval",
							getSendInformJson(username)) ;
					//usleep(100000) ;
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

			//m_face.processEvents(time::milliseconds::zero(),true);
			m_face.processEvents();
		}

	private:

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
			string tmp1 ,tmp2 ;
			cin >> tmp1 ;
			cin >> tmp2 ;
			root["timestamp"] = tmp1+" "+tmp2 ;
			
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
			root["reason"] = "做得不错";

			return root.toStyledString() ;
		}
		string getGetInformJson(string username="wgh2"){
			Json::Value root ;
			root["command"] = "/getInform" ;
			root["username"] = username;
			root["uuid"] = uuid;

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
			detail["content"] = "今天下午，C105 消防演习" ;
			detail["startTime"] = "20191224" ;
			detail["endTime"] = "20191225" ;
			detail["attachment"] = "" ;
			root["detail"] = detail ;

			return root.toStyledString() ;

		}
		string getGetApprovalJson(string username="wgh2"){
			Json::Value root ;
			root["command"] = "/getApproval" ;
			root["username"] = username;
			root["uuid"] = uuid;

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

			if(cmd == 15)
			{
				sleep(1) ;
				mexpressInterest("/ndn/edu/pkusz/OA/getNewsJson", 
						getNewsJson()) ;
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

		string username = "wgh2" ;
		string filename = "wgh2.png" ;

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
