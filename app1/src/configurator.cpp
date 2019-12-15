#include <mmcobj.h>
#include "configurator.h"
#include "rapidjson.h"
#include "document.h"
#include "filereadstream.h"

Configurator::Configurator(::sc_core::sc_module_name)
{
    std::cout << "C-TOR Configurator\n";
    SC_METHOD(do_configurator);
}

void Configurator::do_configurator()
{
    readConfiguration();
}

void Configurator::readConfiguration()
{
    cout << "readConfiguration\n";
    FILE* fp = fopen("../cfg/cfg.json", "rb");

    if (fp == nullptr)
    {
        cout << "fail to open the configuration file\n";
        return;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    cout << "checked is: " << d["checked"].GetBool() ? "true\n" : "false\n";
}
