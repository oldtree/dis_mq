#include "Zhelpers.hpp"

#include <iostream>
#include <thread>
#include <stdlib.h>

using namespace std;

void * step1(void * arg){

	zmq::context_t * context = static_cast<zmq::context_t * >(arg);

	zmq::socket_t sender (*context,ZMQ_PAIR);

	sender.connect("inproc://ont2two");

	s_send(sender,"world");
	return nullptr;
}

void * step2(void * arg){
	zmq::context_t * context = static_cast<zmq::context_t* >(arg);

	zmq::socket_t reciver(*context ,ZMQ_PAIR);

	reciver.bind("inproc://ont2two");

	thread t1(step1,context);
	t1.join();
	cout<<"step2 : "<<s_recv(reciver)<<endl;


	zmq::socket_t sender (*context,ZMQ_PAIR);

	sender.connect("inproc://two2main");
	s_send(sender,"hello");
	return nullptr;


}


void Mtreply1(){
	zmq::context_t context;
	zmq::socket_t reciver(context,ZMQ_PAIR);

	reciver.bind("inproc://two2main") ;
	zmq::socket_t reciver2(context,ZMQ_REQ);
	reciver2.connect("tcp://localhost:5555");
	for (int i =0 ;1<10;i++)
	{
		zmq::message_t req_msg(6);
		zmq::message_t rep_msg;
		thread t1 (step2,(void *) &context);
		t1.join();
		//cout<<"Mtreply : "<<s_recv(reciver)<<endl;
		memcpy((void*)req_msg.data(),s_recv(reciver).c_str(),5);
		reciver2.send(req_msg);

		reciver2.recv(&rep_msg);
		cout<<rep_msg.data()<<endl;
	}
	return;
}


void Mtreply(){
	//  Prepare our context and socket
	zmq::context_t context ;
	zmq::socket_t socket (context, ZMQ_REQ);

	std::cout << "Connecting to hello world server..." << std::endl;
	socket.connect ("tcp://localhost:5555");

	//  Do 10 requests, waiting each time for a response
	for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
		zmq::message_t request (6);
		memcpy ((void *) request.data (), "Hello", 5);
		std::cout << "Sending Hello " << request_nbr << "..." << std::endl;
		socket.send (request);

		//  Get the reply.
		zmq::message_t reply;
		socket.recv (&reply);
		std::cout << "Received World " << request_nbr << std::endl;
	}
	return ;
}











