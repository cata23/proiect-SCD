
// this program implements a simulation of a queuing system:
// - requests are generated randomly (arriving time is random)
// - one queue of infinite length (MAX_INT not really infinite)
// - one or two servers (statically configurable by using macros)
// - serving/processing time is generated randomly 
// - the program computes:
// 		- maximum number of requests in system
//		- requests served by each server
// - TODO:
//		- compute other statistics
//		- change the random distributions for request arival time and 
//			serving time

#include <systemc.h>

// some useful macros for switching simulation mode: one or two servers
#define SIM_ONE_SERVER		1
#define SIM_TWO_SERVER		2

#define SIM_SERVER_MODE		SIM_TWO_SERVER

// the simulation is ended when a specified amount of time has elapsed
// time units in this simulation are expressed in picosecons (SC_PS)
#define SIM_END_TIME		10000.0

// enable/disable debug print; simply comment this line to disable debug print
// #define DEBUG_PRINT_EVENTS

// debug printing displayes some info (chars) associated with events occurrences
#ifdef DEBUG_PRINT_EVENTS	
	#define DEBUG_PRINT( what ) cout<<what;
#else
	#define DEBUG_PRINT( what )
#endif

// events used by simulation for communication between processes
sc_event requestGenerated;
sc_event requestProcessedByServer1;
sc_event requestProcessedByServer2;
sc_event requestArrivedAndSentToServer1;  
sc_event requestTakenFromQueueAndSentToServer1;  
sc_event requestArrivedAndSentToServer2;
sc_event requestTakenFromQueueAndSentToServer2;

// global variables used together with events in order to complete 
//	the communication between processes
// YES, sometimes, events alone are not enough 
//	for implementing the communication!
bool server1IsFree = true;
bool server2IsFree = true;

// some statistics
size_t requestsTotalCount		= 0;
size_t requestsServedByServer1	= 0;
size_t requestsServedByServer2	= 0;
size_t requestsInSystemMax		= 0;

// this module implements the requests generation process
SC_MODULE( req ){

	void do_thd( void ){
		while( true ){
			double time;
			time = rand() % 100 + 1;
			wait( time, SC_PS );
			requestGenerated.notify();
			requestsTotalCount++;
			
			// when simulation time elapsed, print gathered statistics
			if( sc_time_stamp().to_double() > SIM_END_TIME ) {
				cout<<endl;
				cout<<"FINAL STATISTICS"<<endl;
				cout<<"----------------"<<endl;
				cout<<"total requests arrived = "<<requestsTotalCount<<endl;
				cout<<"total requests in system (~ in queue) = "<<requestsInSystemMax<<endl;
				cout<<"requests served by server 1 = "<<requestsServedByServer1<<endl;
				cout<<"requests served by server 2 = "<<requestsServedByServer2<<endl;
				cout<<"----------------"<<endl;
				cout<<endl;

				// here the simulation is stopped
				sc_stop();
			}
		}
	}
	
	SC_CTOR( req ){
		SC_THREAD( do_thd );
	}
};

// this module implements the queue
// the only data that describes the queue is the number of requests in the system: 
//	being served or waiting in queue
// the module has several processes used for request (put) and 
//	service (get) handling  
SC_MODULE( queue ){
	
	size_t requestsInSystem;

	void do_put( void ){

		while( true ){
			wait(requestGenerated);
			DEBUG_PRINT("R");
			if( requestsInSystem == 0 ) {
				DEBUG_PRINT(":");
				requestArrivedAndSentToServer1.notify();		
			}
#if SIM_SERVER_MODE == SIM_TWO_SERVER
			else if ( requestsInSystem == 1 ) {
				DEBUG_PRINT(";");

				if( true == server1IsFree  ) {
					requestArrivedAndSentToServer1.notify();
				} else if( true == server2IsFree ) {
					requestArrivedAndSentToServer2.notify();
				}
			}
#endif
			requestsInSystem++;
			DEBUG_PRINT( requestsInSystem );
			if( requestsInSystem > requestsInSystemMax){
				requestsInSystemMax = requestsInSystem;
			}
		}
	}

	void do_get1( void ){
		while( true ){
			wait( requestProcessedByServer1 );
			if( requestsInSystem ) requestsInSystem--;
			DEBUG_PRINT( requestsInSystem );
#if SIM_SERVER_MODE == SIM_TWO_SERVER
			if( requestsInSystem > 1 ){
#else 
			if( requestsInSystem != 0 ){
#endif
				DEBUG_PRINT(".");
				requestTakenFromQueueAndSentToServer1.notify();
			} else {
				server1IsFree = true;
			}
		}
	}

#if SIM_SERVER_MODE == SIM_TWO_SERVER
	void do_get2( void ){
		while( true ){
			wait( requestProcessedByServer2 );
			if( requestsInSystem ) requestsInSystem--;
			DEBUG_PRINT( requestsInSystem );
			if( requestsInSystem > 1 ){
				DEBUG_PRINT(",");
				requestArrivedAndSentToServer2.notify();
			} else {
				server2IsFree = 1;
			}
		}
	}
#endif

	SC_CTOR( queue ) : requestsInSystem(0) {
		SC_THREAD( do_put );
		SC_THREAD( do_get1 );
#if SIM_SERVER_MODE == SIM_TWO_SERVER
		SC_THREAD( do_get2 );
#endif
	}
	
};

// this module implements the server process(es)
SC_MODULE( serv ){

	void do_thd1( void ){
		while( true ){
			double time;
			wait(requestArrivedAndSentToServer1
				|requestTakenFromQueueAndSentToServer1);
			server1IsFree = 0;
			requestsServedByServer1++;
			time = rand() % 100 + 1;
			wait( time, SC_PS );
			DEBUG_PRINT("~");
			requestProcessedByServer1.notify();
		}
	}

#if SIM_SERVER_MODE == SIM_TWO_SERVER
	void do_thd2( void ){
		while( true ){
			double time;
			wait(requestArrivedAndSentToServer2
				|requestTakenFromQueueAndSentToServer2);
			server2IsFree = 0;
			requestsServedByServer2++;
			time = rand() % 100 + 1;
			wait( time, SC_PS );
			DEBUG_PRINT("-");
			requestProcessedByServer2.notify();
		}
	}
#endif

	SC_CTOR( serv ){
		SC_THREAD( do_thd1 );
#if SIM_SERVER_MODE == SIM_TWO_SERVER
		SC_THREAD( do_thd2 );
#endif
	}

};

int sc_main( int argc, char* argv[] ) {

	req requester_i("requester_i");
	serv server_i("server_i");
	queue queue_i("queue_i");

	sc_start();
	
	return 0;
}