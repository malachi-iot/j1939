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
ArduinoLightingCommandSink ca;


void setup()
{
    Serial.begin(115200);

#if CONFIG_SERIAL_WAIT
    while(!Serial);
#endif

    init_can(t);

#if CONFIG_GPIO_LEFT_BLINKER
    cout << F("GPIO left blinker: ") << CONFIG_GPIO_LEFT_BLINKER << estd::endl;

    pinMode(CONFIG_GPIO_LEFT_BLINKER, OUTPUT);
#endif
#if CONFIG_GPIO_RIGHT_BLINKER
    cout << F("GPIO right blinker: ") << CONFIG_GPIO_RIGHT_BLINKER << estd::endl;

    pinMode(CONFIG_GPIO_RIGHT_BLINKER, OUTPUT);
#endif
#if CONFIG_GPIO_CENTER_STOP_LIGHT
    cout << F("GPIO center stop light: ") << CONFIG_GPIO_CENTER_STOP_LIGHT << estd::endl;

    pinMode(CONFIG_GPIO_CENTER_STOP_LIGHT, OUTPUT);
#endif
    //digitalWrite(CONFIG_GPIO_BJM1_BUTTON1_SINK, CONFIG_BUTTON1_INVERTED ? HIGH : LOW);
}


void loop()
{
    transport::frame f;

    if(t.receive(&f))
    {
        process_incoming(dca, t, f);
        process_incoming(ca, t, f);
    }
}