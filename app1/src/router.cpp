#include "router.h"

Router::Router(::sc_core::sc_module_name)
{
    std::cout << "C-TOR router\n";
    SC_METHOD(do_stuff);
}

void Router::do_stuff()
{
    std::cout << "do stuff function for     router\n";
}
