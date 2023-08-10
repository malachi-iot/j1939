#include <Arduino.h>

#include <estd/chrono.h>
#include <estd/string.h>
#include <estd/tuple.h>

#include <embr/internal/debounce/ultimate.h>

#include <j1939/pdu.h>
#include <j1939/pgn.h>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/ca.hpp>

#include <j1939/ostream.h>

#include <j1939/data_field/oel.hpp>

#include "conf.h"

#include "transport.h"

using namespace estd;
using namespace embr::j1939;
using namespace embr::units::literals;

enum class switches
{
    left_turn = 0,
    right_turn = 1,
    brake = 2,

    MAX
};

namespace debounce = embr::debounce::v1::ultimate;

template <unsigned pin_, bool inverted = false>
struct ArduinoTracker : debounce::Tracker<uint16_t, inverted>
{
    using base_type = debounce::Tracker<uint16_t, inverted>;

    static constexpr const unsigned pin = pin_;

    bool eval()
    {
        // DEBT: We might prefer a more variadic/compile time exclusion here,
        // though I expect this will optimize nicely
        if(pin == 0) return false;

        return base_type::eval(digitalRead(pin));
    }
};


estd::tuple<
    ArduinoTracker<CONFIG_GPIO_LEFT_BLINKER_SWITCH>,
    ArduinoTracker<CONFIG_GPIO_RIGHT_BLINKER_SWITCH>,
    ArduinoTracker<CONFIG_GPIO_MAIN_LIGHT_SWITCH>> switches;

arduino_ostream cout(Serial);

#ifdef AUTOWP_LIB
static transport t(10);     // CS pin
#else
static transport t;
#endif


diagnostic_ca<transport, arduino_ostream> dca(cout);


using States = embr::debounce::v1::States;
using clock = estd::chrono::arduino_clock;


// next scheduled debounce check
static clock::time_point next;


void setup()
{
    Serial.begin(115200);

    while(!Serial);

    init_can(t);

    next = clock::now();
}


spn::measured convert_to_measured(embr::debounce::v1::States s)
{
    using S = embr::debounce::v1::States;

    switch(s)
    {
        case S::On:     return spn::measured::enabled;
        case S::Off:    return spn::measured::disabled;
        default:        return spn::measured::noop;
    }
}


void loop()
{
    transport::frame f;
    using traits = transport_traits<transport>;

    pdu<pgns::oel> pdu;

    if(t.receive(&f))
    {
        process_incoming(dca, t, f);
    }

    // if we haven't yet reached timeout to check debounce status, abort
    // and re-loop
    if(clock::now() < next) return;

    next += 10ms;

    // FIX: estd has a bug where get<1> and down the line are ambiguous

    bool v = get<0>(switches).eval();
    //get<1>(switches).eval();
    //get<2>(switches).eval();

    if(v)
    {
        using et = enum_type<spns::turn_signal_switch>;

        States left_state = get<0>(switches).state();
        States right_state = States::Undefined; // get<1>(switches).state();

        if(left_state == States::On && right_state == States::On)
        {
            // UNDEFINED, though might be good for hazard lights?
        }

        if(left_state == States::On)
            pdu.turn_signal_switch(et::left_turn_to_be_flashing);
        else if(right_state == States::On)
            pdu.turn_signal_switch(et::right_turn_to_be_flashing);

        traits::send(t, pdu);
    }
}