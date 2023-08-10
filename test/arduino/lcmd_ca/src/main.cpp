#include <Arduino.h>

#include <estd/chrono.h>
#include <estd/string.h>
#include <estd/tuple.h>

#include <j1939/pdu.h>
#include <j1939/pgn.h>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/ca.hpp>

#include <j1939/ostream.h>

#include <j1939/data_field/oel.hpp>

#include "ca.h"
#include "conf.h"

#include "transport.h"

using namespace estd;
using namespace embr::j1939;



arduino_ostream cout(Serial);

#ifdef AUTOWP_LIB
static transport t(10);     // CS pin
#else
static transport t;
#endif


scheduler_type scheduler;

#if CONFIG_DIAGNOSTIC_CA
diagnostic_ca<transport, arduino_ostream> dca(cout);
#endif

ArduinoLightingCommandCa ca(scheduler);

using clock = estd::chrono::arduino_clock;


void setup()
{
    Serial.begin(115200);

#if CONFIG_SERIAL_WAIT
    while(!Serial);
#endif

    cout << F("LCMD CA") << estd::endl;

    init_can(t);
}


void loop()
{
    scheduler.process();

    transport::frame f;

    if(t.receive(&f))
    {
#if CONFIG_DIAGNOSTIC_CA
        process_incoming(dca, t, f);
#endif
        process_incoming(ca, t, f);
    }
}