#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <zmq.hpp>
#include "Zhelpers.hpp"

//  Asynchronous client-to-server (DEALER to ROUTER)
//
//  While this example runs in a single process, that is to make
//  it easier to start and stop the example. Each task has its own
//  context and conceptually acts as a separate process.
//
// 

//  This is our client task class.
//  It connects to the server, and then sends a request once per second
//  It collects responses as they arrive, and it prints them out. We will
//  run several client tasks in parallel, each with a different random ID.
//  Attention! -- this random work well only on linux.


class client_task
{
	private:
		zmq::context_t ctx_;
		zmq::socket_t client_socket_;
public:
	client_task() :ctx_(),client_socket_(ctx_,ZMQ_DEALER){

	}

	void start(){
		// generate random identity

		char identity[10] = {};
		sprintf(identity,"%04x-%04x",within(0x10000),within(0x10000));

		printf("%s \n",identity);
		client_socket_.setsockopt(ZMQ_IDENTITY,identity,strlen(identity));
		client_socket_.connect("tcp://localhost:5570");
		zmq::pollitem_t items[]= {{client_socket_,0,ZMQ_POLLIN}};
		int request_nbr = 0;

		try{
			while (true)
			{
				for (int i = 0; i < 100; i++)
				{
					zmq::poll(items,1,10);
					if (items[0].revents & ZMQ_POLLIN)
					{
						printf("\n %s",identity);
						s_dump(client_socket_);
					}
				}
				char request_string[16] = {};
				sprintf(request_string,"request  #%d ",++request_nbr);
				client_socket_.send(request_string,strlen(request_string));
			}

		}catch(std::exception & e){
			

		}

	}

protected:


};


//  .split worker task
//  Each worker task works on one request at a time and sends a random number
//  of replies back, with random delays between replies:

class server_worker
{
	private:
		zmq::context_t & ctx_;
		zmq::socket_t  worker_;
public:
	server_worker(zmq::context_t & ctx ,int sock_type):ctx_(ctx),worker_(ctx_,sock_type){

	}
	void work(){
		worker_.connect("inproc://beackend");

		try
		{
			while (true)
			{
				zmq::message_t identity;
				zmq::message_t msg;
				worker_.recv(&identity);
				worker_.recv(&msg);
				int replies = within(5);
				for (int reply =0; reply< replies;++reply)
				{
					s_sleep(within(1000)+1);
					worker_.send(identity,ZMQ_SNDMORE);
					worker_.send(msg);
				}

			}
		}
		catch (std::exception* e)
		{
			std::cout<<e->what()<<std::endl;
		}
	}
protected:

};

//  .split server task
//  This is our server task.
//  It uses the multithreaded server model to deal requests out to a pool
//  of workers and route replies back to clients. One worker can handle
//  one request at a time but one client can talk to multiple workers at
//  once.

class server_task
{
	private:
		zmq::context_t ctx_;
		zmq::socket_t frontend_;
		zmq::socket_t backend_;
public:
	server_task():frontend_(ctx_,ZMQ_ROUTER),backend_(ctx_,ZMQ_DEALER){

	}
	void run(){
		frontend_.bind("tcp://*:5570");
		backend_.bind("inproc://backend");

		std::vector<server_worker *>worker;
		std::vector<std::thread *> worker_thread;
		enum{
			kMaxThread = 5,
		};
		for (int i = 0; i < kMaxThread; i++)
		{
			worker.push_back(new server_worker(ctx_,ZMQ_DEALER));
			worker_thread.push_back(new std::thread(std::bind(&server_worker::work,worker[i])));
			worker_thread[i]->detach();
		}


		try
		{
			zmq::proxy(frontend_,backend_,nullptr);
		}
		catch (std::exception* e)
		{
			
		}
	}
protected:

};


int asyncsrv(){
	client_task ct1;
	client_task ct2;
	client_task ct3;

	server_task st;
	std::thread t1(std::bind(&client_task::start,&ct1));
	std::thread t2(std::bind(&client_task::start,&ct2));
	std::thread t3(std::bind(&client_task::start,&ct3));
	std::thread t4(std::bind(&server_task::run,&st));

	t1.detach();
	t3.detach();
	t2.detach();

	getchar();
	return 0;
}
