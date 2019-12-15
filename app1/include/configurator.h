#ifndef APP1_CONFIGURATOR_H
#define APP1_CONFIGURATOR_H
#include "systemc.h"

SC_MODULE(Configurator )
{
    // variable for Configurator

    /**
     * ctor
     */
    SC_CTOR (Configurator);

    /**
     * @brief initial configurations
     */
    void do_configurator();

    /**
     * @brief read configuration form file
     */
    void readConfiguration();



};


#endif //APP1_CONFIGURATOR_H
