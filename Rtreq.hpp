#include "Zhelpers.hpp"

#include <string>
#include <iostream>
#include <stdlib.h>
#include <thread>

using namespace std;

#define  NBR_WORKERS 10

static void * worker_thread(void *arg){
	 zmq::context_t * context = (zmq::context_t*)arg;
	 zmq::socket_t worker(*context,ZMQ_REQ);
	  //  We use a string identity for ease here
	 s_set_id(worker);

	 worker.bind("ipc://routing.ipc");

	 int total = 0;
	 while (true)
	 {
		 //  Tell the router we're ready for work
		 s_send(worker,"ready");

		 //  Get workload from router, until finished
		 std::string workload = s_recv(worker);

		 int finished = (workload.compare("END") == 0);

		 if (finished)
		 {
			 std::cout<<"Processed :"<<total<<" task "<<std::endl;
			 break;
		 }

		 total ++;

		 s_sleep(within(100)+1);

		 return nullptr;
	 }
}

void rpc_client(){
	zmq::context_t context ;
	zmq::socket_t client (context,ZMQ_ROUTER);
	client.bind("ipc://routing.ipc");

	int worker_nbr;
	for (worker_nbr= 0; worker_nbr < NBR_WORKERS; worker_nbr++)
	{
		thread worker(worker_thread,&context) ;
	}

	int task_nbr;

	for (task_nbr = 0; task_nbr < NBR_WORKERS; task_nbr++)
	{

		string address = s_recv(client);
		{
			s_recv(client);
			s_recv(client);
		}
		s_sendmore(client,address);
		s_sendmore(client,"");
		s_send(client,"this is the work load");

	}

	for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr++)
	{

		std::string address = s_recv(client);
		{
			s_recv(client);
			s_recv(client);
		}

		s_sendmore(client,address);
		s_sendmore(client,"");
		s_send(client,"END");
	}

	Sleep(1);
	return ;

}
