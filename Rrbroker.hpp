#include "Zhelpers.hpp"
#include <thread>
#include <iostream>
#include <string>
using namespace std;

void worker_(){
	zmq::context_t context;
	zmq::socket_t responder(context,ZMQ_REP);
	responder.connect("tcp://localhost:5560");
	while (true)
	{
		zmq::message_t req_msg;
		zmq::message_t rep_msg(5);
		//  Wait for next request from client
		//std::string string = s_recv(responder);
		responder.recv(&req_msg);
		std::cout<<" received request :"<<(char *)req_msg.data() <<std::endl;

		// Do some 'work'
		Sleep(1);

		//  Send reply back to client
		memcpy ((void *) rep_msg.data (), "World", 5);
		responder.send(rep_msg);

	}

}
void Rrbroker(){
	zmq::context_t context ;
	zmq::socket_t frontend(context,ZMQ_ROUTER);
	zmq::socket_t backend(context,ZMQ_DEALER);

	frontend.bind("tcp://*:5559");
	backend.bind("tcp://*:5560");

	zmq::pollitem_t items[]={
		{frontend,0,ZMQ_POLLIN,0},{backend,0,ZMQ_POLLIN,0}
	} ;
	//  Switch messages between sockets
	while (true)
	{
		cout<<"fuck "<<endl;
	    zmq::message_t message ;
		//string data;
		//int64_t more;
		unsigned int more;
		zmq::poll(&items[0],2,-1);

		if (items[0].revents & ZMQ_POLLIN)
		{
			cout<<" items[0]  "<<endl;
			while (1)
			{
				//  Process all parts of the message..
				frontend.recv(&message);
				//data = s_recv(frontend);
				size_t more_size = sizeof(more);
				frontend.getsockopt(ZMQ_RCVMORE,&more,&more_size);
				backend.send(message,more?ZMQ_SNDMORE:0);
				//s_send(backend,data);
				if (!more)
				{
					
					break;
				}
			}
		}

		if (items[1].revents & ZMQ_POLLIN)
		{
			cout<<" items[1]  "<<endl;
			while (1)
			{
				//  Process all parts of the message

				backend.recv(&message);
				//data = s_recv(backend);
				size_t  more_Size = sizeof(more);
				backend.getsockopt(ZMQ_RCVMORE,&more,&more_Size);
				frontend.send(message,more?ZMQ_SNDMORE:0);
				//s_send(frontend,data);
				if (!more)
				{
					
					break;
				}
			}
		}
	}
	return ;
}

#if 1
void worker_Rrbroker()	{
	thread t1(worker_);
	thread t2(Rrbroker);

	t2.join();
	t1.detach();
	//t1.join();
	//t2.join();
}  
#endif // 1
