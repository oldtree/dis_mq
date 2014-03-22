#include "Zhelpers.hpp"
#include <iostream>
#include <string>

void pub_server(){
	zmq::context_t context;
	zmq::socket_t publisher (context,ZMQ_PUB);

	publisher.bind("tcp://*:5563");
	while (true)
	{
		s_sendmore(publisher,"A");
		s_send(publisher," We don't want to see this ");
		s_sendmore(publisher,"B");
		s_send(publisher," We would like to see this ") ;
		Sleep(2000);
	}
}
