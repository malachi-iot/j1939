#include <Arduino.h>

#include <math.h>

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

#include <j1939/units/percent.h>
#include <embr/units/volts.h>
#include <estd/internal/units/si.h>

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

constexpr embr::word<3> ecu_instance(0);

nca_type nca(proto_name::sparse{0, 0, ecu_instance.value()}, scheduler);

}

void setup()
{
    Serial.begin(115200);

#if CONFIG_SERIAL_WAIT
    while(!Serial);
#endif

    cout << F("CM1 Source") << estd::endl;

    init_can(t);

    // Try to grab an address right from the start
    app::nca.start(t);
}

// presuming 10-bit resolution and no voltage correction curves
using adc_volts = embr::units::volts<int16_t,
    estd::ratio<(int)(CONFIG_LOGIC_VOLTAGE * 10), 1024 * 10> >;

// i.e. // 512/1024 = 0.5 then we need * 100
using adc_percent = estd::internal::units::percent<int16_t, estd::ratio<100, 1024> >;


void loop()
{
    static adc_percent last_read{-1};

    using traits = transport_traits<transport>;

    scheduler.process();

    adc_percent v(analogRead(CM1_IO_FAN_POT));

    // DEBT: doesn't work if I get rid of 'mv' temporary variable
    adc_volts mv(v.count());
    embr::units::millivolts<int16_t> mv2(mv);

    // meager thinning data - we need the equivelant of a debounce here,
    // perhaps a true blue smoothing/LP filter
    if(abs(last_read.count() - v.count()) > 10)
    {
        last_read = v;

        pdu<pgns::cab_message1> pdu;

        if(app::nca.has_address())
            pdu.source_address(*app::nca.address());

        pdu.destination_address(0xFF);

        pdu.requested_percent_fan_speed(v);

        traits::send(t, pdu);

        cout << F("reading: ") << estd::put_unit(mv2) << F("/");
        cout << v.count() << F("/");
        // DEBT: Curiously, we can put <double> here but not <float>, estd's
        // ostream gets confused.  Doing neither just to save a few bytes
        cout << estd::put_unit(estd::internal::units::percent<int16_t>(v)) << endl;
    }

    transport::frame f;

    if(t.receive(&f))
    {
#if CONFIG_DIAGNOSTIC_CA
        process_incoming(app::dca, t, f);
#endif
        process_incoming(app::nca, t, f);
    }

    // Does use about 6 bytes less memory on AVR...
    //cout << F("reading: ") << mv2.count() << F("mv ") << endl;

    //cout << F("reading: ") << embr::put_unit(mv2) << endl;

    // No extra code to thin out CAN output, so theoretically we could spam
    // the bus with a continuous 100 messages/s
    this_thread::sleep_for(10ms);
}