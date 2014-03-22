#include <thread>
#include <string>
#include <iostream>
#include <assert.h>
#include <zmq.hpp>

#include "Zhelpers.hpp"
using namespace std;

void * worker_rountine(void * arg){

	zmq::context_t * context = static_cast<zmq::context_t*>(arg);
	zmq::socket_t  socket(*context,ZMQ_REP);
	socket.bind("inproc://workers");
	while (true)
	{
		zmq::message_t request ;
		socket.recv(&request);
		std::cout << "Received request: [" << (char*) request.data() << "]" << std::endl;

		Sleep(1);

		zmq::message_t reply (5);
		memcpy((void * )reply.data(),"world",5);
		socket.send(reply);

	}
	return nullptr;

}

void Mtserver(){
	zmq::context_t  context;

	zmq::socket_t clients(context,ZMQ_ROUTER);
	clients.bind("tcp://*:5555");
	zmq::socket_t workers(context,ZMQ_DEALER);

	workers.connect("inproc://workers");

	//for (int i = 0; i < 5; i++)
	//{
	//	thread worker(worker_rountine,(void * )&context) ;
	//	worker.join();
	//}
	zmq::proxy(clients,workers,nullptr)	;
}



