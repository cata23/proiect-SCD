#ifndef APP1_ROUTER_H
#define APP1_ROUTER_H

#include <systemc.h>
#include "types.h"

SC_MODULE( Router )
{
    // out ports
    sc_export< sc_fifo_out_if< Event > > portInN;
    sc_export< sc_fifo_out_if< Event > > portInS;
    sc_export< sc_fifo_out_if< Event > > portInV;
    sc_export< sc_fifo_out_if< Event > > portInE;
    // in ports
    sc_export< sc_fifo_in_if< Event > > portOutN;
    sc_export< sc_fifo_in_if< Event > > portOutS;
    sc_export< sc_fifo_in_if< Event > > portOutV;
    sc_export< sc_fifo_in_if< Event > > portOutE;


    /**
     * ctor
     */
    SC_CTOR(Router);

    /**
     * @brief just    for test
     */
    void do_stuff();


};


#endif //APP1_ROUTER_H
