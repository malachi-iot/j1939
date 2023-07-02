#include <Arduino.h>

#include <estd/istream.h>
#include <estd/ostream.h>
#include <estd/string.h>

#include <j1939/pdu.h>
#include <j1939/pgn.h>

#include <j1939/data_field/all.hpp>

// DEBT: Eventually we'd like to auto include this
#include <j1939/units/ostream.h>

#include "transport.h"

using namespace estd;
using namespace embr::j1939;
using namespace embr::units::literals;

arduino_ostream cout(Serial);
arduino_istream cin(Serial);

#ifdef AUTOWP_LIB
static transport t(10);     // CS pin
#else
static transport t;
#endif


void setup()
{
    Serial.begin(115200);

    while(!Serial);

    init_can(t);
}


void loop()
{

}