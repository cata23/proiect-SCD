
#include "systemc.h"

sc_time endTime(100, SC_NS);
sc_time routerCycleTime( 10, SC_NS);

sc_fifo< int > fifoInLeft;
sc_fifo< int > fifoInRight;

sc_fifo< int > fifoOutLeft;
sc_fifo< int > fifoOutRight;

SC_MODULE( router ) {
	sc_port< sc_fifo_in_if< int > > portInLeft;
	sc_port< sc_fifo_in_if< int > > portInRight;

	sc_port< sc_fifo_out_if< int > > portOutLeft;
	sc_port< sc_fifo_out_if< int > > portOutRight;


	void routerThd( void ){
		int data;
		int leftNum;
		int rightNum;

		while( true ){
			wait( routerCycleTime );
			
			portOutLeft->write( portInRight->read() );
			portOutRight->write( portInLeft->read() );

		}
	}
	
	SC_CTOR( router ){

		SC_THREAD( routerThd );
	}
};

SC_MODULE( gen ) {

	sc_port< sc_fifo_out_if< int > > portA;
	sc_port< sc_fifo_out_if< int > > portB;

	void genThd( void ){
		int x;
		while( true ){ 
			wait( routerCycleTime );
			
			x = rand()%100 + 1;

			portA->write(x);
			portB->write(x + 1);
			
			cout << sc_time_stamp() << " GEN : " << x << " " << x+1 << endl;
			if( sc_time_stamp() == endTime ){
				sc_stop();
			}
		}
	}
	
	SC_CTOR( gen ) {
		SC_THREAD( genThd );
	}
};

SC_MODULE( mon ) {

	sc_port< sc_fifo_in_if< int > > portA;
	sc_port< sc_fifo_in_if< int > > portB;

	void monThd( void ) {
		while( true ) {
			wait( portA->data_written_event() | portB->data_written_event() );
			if( portA->num_available() ) {
				cout<<sc_time_stamp()<<" MON PortA : "<<portA->read()<<endl;
			}
			if( portB->num_available() ) {
				cout<<sc_time_stamp()<<" MON PortB : "<<portB->read()<<endl;
			}
		}
	}

	SC_CTOR( mon ) {
		SC_THREAD( monThd );
	}

};
	
int sc_main( int argc, char* argv[] ) {

	router router_i( "router" );
	
	gen gen_i( "gen" );
	mon mon_i( "mon" );

	router_i.portInLeft(fifoInLeft);
	router_i.portInRight(fifoInRight);
	router_i.portOutLeft(fifoOutLeft);
	router_i.portOutRight(fifoOutRight);

	gen_i.portA(fifoInLeft);
	gen_i.portB(fifoInRight);

	mon_i.portA(fifoOutLeft);
	mon_i.portB(fifoOutRight);

	sc_start();

	return 0;
}