#include <iostream>
#include "dns_client.h"

using namespace std;

int main()
{
	DNS_Client &mDNSClient = DNS_Client::getInstance() ;


	int i = 0 ;
	while(1)
	{
		auto ret = mDNSClient.connectServer() ;
		cout << i++ << endl ;
		cout << ret.first << endl ;
		cout << ret.second << endl ;
		//if(i > 1000) cin >> i ;
		usleep(10000) ;
		if(ret.first < 0) break ;
	}

	return 0;
}
