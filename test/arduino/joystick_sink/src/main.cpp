#include <Arduino.h>

#include <estd/string.h>

#include <j1939/pdu.h>
#include <j1939/pgn.h>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/ca.hpp>

#include <j1939/data_field/all.hpp>

#include <j1939/ostream.h>

#include "transport.h"
#include "ca.h"

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
ArduinoJoystickSink ca;

// Inverted = active low


void setup()
{
    Serial.begin(115200);

    while(!Serial);

    init_can(t);

    pinMode(CONFIG_GPIO_BJM1_BUTTON1_SINK, OUTPUT);
    digitalWrite(CONFIG_GPIO_BJM1_BUTTON1_SINK, CONFIG_BUTTON1_INVERTED ? HIGH : LOW);
}


void loop()
{
    transport::frame f;

    if(t.receive(&f))
    {
        process_incoming(dca, t, f);
        process_incoming(ca, t, f);
    }

    /*
    delay(500);
    digitalWrite(CONFIG_GPIO_BUTTON1, HIGH);
    delay(500);
    digitalWrite(CONFIG_GPIO_BUTTON1, LOW); */
}