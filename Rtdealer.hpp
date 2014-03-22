#include "Zhelpers.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <zmq.hpp>
using namespace  std;

void * worker_a(void * arg){
	zmq::context_t * context = (zmq::context_t * )arg;
	zmq::socket_t worker(*context,ZMQ_DEALER);
	worker.setsockopt(ZMQ_IDENTITY,"A",1);
	worker.connect("ipc://routing.ipc");

	int total = 0;
	while (1)
	{
		std::string request = s_recv(worker);
		int finished = (request.compare("END") == 0);
		if (finished)
		{
			std::cout<<"A recevied :"<<total << std::endl;
			break;
		}

		total++;
	}
	return nullptr;
}


void * worker_b(void * arg){
    zmq::context_t *context =  (zmq::context_t* )arg;
	zmq::socket_t worker (*context,ZMQ_DEALER);
	worker.setsockopt(ZMQ_IDENTITY,"B",1);
	worker.connect("ipc://rounting.ipc");

	int total = 0;
	while (1)
	{
		string request = s_recv(worker);
		int finished = (request.compare("END") == 0);

		if (finished)
		{
			cout<<"B recevied :"<<total<<std::endl;
		}
		total ++;
	}
	return nullptr;
}

void ipc_server(){
	zmq::context_t context;
	zmq::socket_t client (context,ZMQ_ROUTER);
	client.bind("ipc://grouting.ipc");
    thread worker1(worker_a,&context) ;
	thread worker2(worker_b,&context) ;

	s_sleep(1);

	int task_nbr =0 ;

	srand((unsigned)time(nullptr));

	for (task_nbr = 0;task_nbr<10;task_nbr++)
	{
		if (within(3) > 0)
		{
			s_sendmore(client,"A");
		} 
		else
		{
			s_sendmore(client,"B");
		}
		s_send(client,"this is the workload");
	}

	s_sendmore(client,"A");
	s_send(client,"END");

	s_sendmore(client,"B");
	s_send(client,"END");
	s_sleep(1);
	return;

	
}
