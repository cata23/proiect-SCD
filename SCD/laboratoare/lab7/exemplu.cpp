
#include <systemc.h>

// must be 1 or 2
#define ROUTERS_NO	1

sc_time endTime(100, SC_NS);
sc_time routerCycleTime( 10, SC_NS);

#if ROUTERS_NO == 2
SC_MODULE( link ) {
	sc_port< sc_fifo_in_if< int > > portIn;
	sc_port< sc_fifo_out_if< int > > portOut;

	void linkThd( void ){
		while( true ){
			// TODO is this wait correct?
			wait( portIn->data_written_event() );			
			portOut->write( portIn->read() );
		}
	}
	
	SC_CTOR( link ){
		SC_THREAD( linkThd );
	}
};
#endif

SC_MODULE( router ) {
	sc_export< sc_fifo_out_if< int > > portInLeft;
	sc_export< sc_fifo_out_if< int > > portInRight;

	sc_export< sc_fifo_in_if< int > > portOutLeft;
	sc_export< sc_fifo_in_if< int > > portOutRight;

	sc_fifo< int > fifoInLeft;
	sc_fifo< int > fifoInRight;

	sc_fifo< int > fifoOutLeft;
	sc_fifo< int > fifoOutRight;

	void routerThd( void ){
		int data;
		int leftNum;
		int rightNum;

		while( true ){
			wait( routerCycleTime );
			
			leftNum = fifoInLeft.num_available();
			if( leftNum ) {
				fifoInLeft.nb_read( data );
				fifoOutRight.write( data );
			}

			rightNum = fifoInRight.num_available();
			if( rightNum ) {
				fifoInRight.nb_read( data );
				fifoOutLeft.write( data );
			}
		}
	}
	
	SC_CTOR( router ){
		portInLeft( fifoInLeft );
		portInRight( fifoInRight );

		portOutLeft( fifoOutLeft );
		portOutRight( fifoOutRight );

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
	router router1_i( "router1" );

	gen gen_i( "gen" );
	mon mon_i( "mon" );

#if ROUTERS_NO == 2
	link l1_i( "link1" );
	link l2_i( "link2" );
#endif

	gen_i.portA( router_i.portInLeft );
	gen_i.portB( router_i.portInRight );

#if ROUTERS_NO == 2
	l1_i.portIn( router_i.portOutLeft );
	l1_i.portOut( router1_i.portInLeft );

	l2_i.portIn( router_i.portOutRight );
	l2_i.portOut( router1_i.portInRight );

	mon_i.portA( router1_i.portOutLeft );
	mon_i.portB( router1_i.portOutRight );
#else
	mon_i.portA( router_i.portOutLeft );
	mon_i.portB( router_i.portOutRight );
#endif

	sc_start();

	return 0;
}