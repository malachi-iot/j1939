#include <Arduino.h>

#include <estd/string.h>

#include <j1939/pdu.h>
#include <j1939/pgn.h>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/ca.hpp>

#include <j1939/data_field/all.hpp>

#include <j1939/ostream.h>

#include "transport.h"

using namespace estd;
using namespace embr::j1939;
using namespace embr::units::literals;

arduino_ostream cout(Serial);

#ifdef AUTOWP_LIB
static transport t(10);     // CS pin
#else
static transport t;
#endif


diagnostic_ca<transport, arduino_ostream> dca(cout);


void setup()
{
    Serial.begin(115200);

    while(!Serial);

    init_can(t);
}


void loop()
{
    transport::frame f;

    if(t.receive(&f))
    {
        // overflows attiny
        process_incoming(dca, t, f);
    }
}