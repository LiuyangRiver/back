#include <iostream>
#include "ndn/ndnserver.h"

using namespace ndn ;
using namespace std ;
int main()
{
	string prefix = "/ndn/edu/pkusz/OA" ;
	cout << "listen on : " << prefix << endl ;
	NDNServer mNDNServer(prefix) ;
	mNDNServer.start() ;

	return 0;
}
