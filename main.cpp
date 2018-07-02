
#include <atomic>
#include "Broker.h"
#include "TcpClient.h"
#include <iostream>
#include <boost/thread/future.hpp>
// #include "TestString.pb.h"
 using namespace std;

 atomic<bool> start = false;
 using namespace com::tonyemu::proto;
 int main()
 {
 	context_b context;
 	shared_socket ss(new socket_b(context));
 	context_b::work work(context);

 	KeepLiveServer keeplive("0.0.0.0",5579,3500,3);
 	keeplive.setOnlost([](vector<shared_socket>& lost){
 		for (auto iter = lost.begin(); iter != lost.end(); ++iter)
 		{
 			
 			cout << iter->get()->remote_endpoint() << endl;
 		}
 	});
 	keeplive.start(2);
 	KeepLiveClient kc(ss,3000);
 	kc.start("127.0.0.1",5579);
 	context.run();
 	system("pause");
 	keeplive.stop();
 	kc.stop();
 	return 0;
 }
