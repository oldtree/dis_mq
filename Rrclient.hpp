#include "Zhelpers.hpp"

#include <iostream>
#include <stdlib.h>
#include <string>

#include <thread>
using namespace std;

void Rrckient(){
	zmq::context_t context;

	zmq::socket_t requester(context,ZMQ_REQ);

	requester.connect("tcp://localhost:5559");

	for (int request = 0; request < 1000; request++)
	{
		std::cout<<"Received reply  "<<request;
		s_send(requester,"hello");
		std::string string = s_recv(requester);
		std::cout<<" [ "<<string<<" ]  "<<endl;
	}
}

void testfor(){
	for (int i = 0; i < 200; i++)
	{
	   thread t1(Rrckient);
	   t1.join();
	}
}
