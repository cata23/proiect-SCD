
#include <systemc.h>

#include <iostream>
#include <iomanip>

using namespace std;

SC_MODULE(nand2)          
{
	sc_in<bool> ina, inb;       
	sc_out<bool> out;         

	void do_nand2() 
	{
		out.write( !(ina.read() && inb.read()) );
	}
	
	SC_CTOR(nand2) 
	{
		SC_METHOD(do_nand2); // do_not_initialize();  
		sensitive << ina << inb;  
	}
};

SC_MODULE(exor2)
{
	sc_in<bool> ina, inb;
	sc_out<bool> out;

	nand2 n1, n2, n3, n4;

	sc_signal<bool> S1, S2, S3;

	SC_CTOR(exor2) : n1("gate1"), n2("gate2"), n3("gate3"), n4("gate4")
	{
		n1.ina(ina);
		n1.inb(inb);
		n1.out(S1);

		n2.ina(ina);
		n2.inb(S1);
		n2.out(S2);

		n3.ina(S1);
		n3.inb(inb);
		n3.out(S3);

		n4.ina(S2);
		n4.inb(S3);
		n4.out(out);
  }
};

SC_MODULE(stim)
{
	sc_out<bool> A, B;
	sc_in<bool> Clk;

	void StimGen()
	{
		A.write(false);
		B.write(false);
		wait();
		A.write(false);
		B.write(true);
		wait();
		A.write(true);
		B.write(false);
		wait();
		A.write(true);
		B.write(true);
		wait();
		A.write(false);
		B.write(false);
		wait();

		sc_stop();
	}

	SC_CTOR(stim)
	{
		SC_THREAD(StimGen);
		sensitive << Clk.pos();
	}
};

SC_MODULE(mon)
{
	sc_in<bool> A,B,F;
	sc_in<bool> Clk;

	void monitor()
	{
		cout << setw(10) << "Time";
		cout << setw(2) << "A" ;
		cout << setw(2) << "B";
		cout << setw(2) << "F" << endl;
		while (true) {
			cout << setw(10) << sc_time_stamp();
			cout << setw(2) << A.read();
			cout << setw(2) << B.read();
			cout << setw(2) << F.read() << endl;
			wait();
		}
	}

	SC_CTOR(mon)
	{
		SC_THREAD(monitor);
		sensitive << Clk.pos();
	}
};

int sc_main(int argc, char* argv[]) 
{
	sc_signal<bool> ASig, BSig, FSig;
	sc_clock TestClk("TestClock", 10, SC_NS,0.5);

	stim Stim1("Stimulus");
	Stim1.A(ASig);
	Stim1.B(BSig);
	Stim1.Clk(TestClk);

	exor2 DUT("exor2");
	DUT.ina(ASig);
	DUT.inb(BSig);
	DUT.out(FSig);

	mon Monitor1("Monitor");
	Monitor1.A(ASig);
	Monitor1.B(BSig);
	Monitor1.F(FSig);
	Monitor1.Clk(TestClk);

	sc_trace_file* Tf;
	Tf = sc_create_vcd_trace_file("traces");
	Tf->set_time_unit(1, SC_NS);
	sc_trace(Tf, ASig  , "A" );
	sc_trace(Tf, BSig  , "B" );
	sc_trace(Tf, FSig  , "F" );
	sc_trace(Tf, DUT.S1, "S1");
	sc_trace(Tf, DUT.S2, "S2");
	sc_trace(Tf, DUT.S3, "S3");
	sc_trace(Tf, TestClk, "Clk");

	sc_start();
	sc_close_vcd_trace_file(Tf);	

	return 0;
}
