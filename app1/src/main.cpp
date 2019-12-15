#include "systemc.h"
#include "configurator.h"
#include "router.h"

//SC_MODULE(Test)
//{
//    void sim()
//    {
//        printf("Hello World\n");
//    }
//
//    SC_CTOR(Test)
//    {
//        SC_METHOD(sim); sensitive << clk.pos();
//    }
//
//    sc_in<bool> clk;
//};

int sc_main(int argc, char *argv[])
{
//    sc_clock clk("clk", 1, SC_NS);
//
//    Test t("test");
//    t.clk(clk);
//
//    sc_start(10, SC_NS);

    Configurator c("configurator");

    Router r1("router 1");

    sc_start(10, SC_NS);

    return 0;
}