#include <Arduino.h>

#include <estd/chrono.h>
#include <estd/string.h>
#include <estd/tuple.h>
#include <estd/thread.h>

#include <j1939/pdu.h>
#include <j1939/pgn.h>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/ca.hpp>

#include <j1939/ostream.h>

#include <j1939/data_field/oel.hpp>
#include <j1939/units/volts.h>
#include <j1939/units/si.h>

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

using namespace estd::chrono_literals;

namespace app {

// M4 gets understandably mad, we collide with
// https://pubs.opengroup.org/onlinepubs/9699919799/functions/clock.html
// so isolate into app namespace
using clock = estd::chrono::arduino_clock;

#if CONFIG_DIAGNOSTIC_CA
diagnostic_ca<transport, arduino_ostream> dca(cout);
#endif

}

void setup()
{
    Serial.begin(115200);

#if CONFIG_SERIAL_WAIT
    while(!Serial);
#endif

    cout << F("CM1 Source") << estd::endl;

    init_can(t);
}

// presuming 10-bit resolution and no voltage correction curves
using adc_volts = embr::units::volts<int16_t,
    estd::ratio<(int)(CONFIG_LOGIC_VOLTAGE * 10), 1024 * 10> >;


void loop()
{
    scheduler.process();

    int16_t v = analogRead(CM1_IO_FAN_POT);

    // DEBT: doesn't work if I get rid of 'mv' temporary variable
    adc_volts mv(v);
    embr::units::millivolts<int16_t> mv2(mv);

    transport::frame f;

    if(t.receive(&f))
    {
#if CONFIG_DIAGNOSTIC_CA
        process_incoming(app::dca, t, f);
#endif
    }

    // Does use about 6 bytes less memory on AVR...
    //cout << F("reading: ") << mv2.count() << F("mv ") << endl;
    cout << F("reading: ") << embr::put_unit(mv2) << endl;

    this_thread::sleep_for(100ms);
}