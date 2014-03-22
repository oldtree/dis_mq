#include "Zhelpers.hpp"
#include <iostream>
#include <string>
#include <thread>
void sub_client(){
	zmq::context_t context;
	zmq::socket_t subscriber(context,ZMQ_SUB);
	zmq::socket_t subscriber1(context,ZMQ_SUB);
	subscriber.connect("tcp://localhost:5563");
	subscriber1.connect("tcp://localhost:5563");
	subscriber.setsockopt(ZMQ_SUBSCRIBE,"B",1);
	subscriber1.setsockopt(ZMQ_SUBSCRIBE,"A",1);
	while (true)
	{
		std::string address = s_recv(subscriber);
		std::string address1 = s_recv(subscriber1);
		//std::string contents = s_recv(subscriber);
		string contents("help");
		Sleep(2000);
		std::cout<<"sub_client  :  ["<<address <<"] "<<contents<<std::endl;
		std::cout<<"sub_client  :  ["<<address1 <<"] "<<contents<<std::endl;
	}
}
