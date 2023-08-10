#include <Arduino.h>

#include <estd/chrono.h>
#include <estd/string.h>
#include <estd/tuple.h>

#undef max
#include <embr/internal/bitset.h>
#include <embr/platform/arduino/debounce.h>

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

enum Switches
{
    SWITCH_LEFT_TURN = 0,
    SWITCH_RIGHT_TURN,
    SWITCH_MAIN,

    SWITCH_MAX
};

namespace debounce = embr::debounce::v1::ultimate;

template <int pin_, bool inverted = false>
using Debouncer = embr::arduino::debounce::v1::ultimate::Debouncer<(unsigned)pin_, inverted>;


#if CONFIG_GPIO_LEFT_BLINKER_SWITCH == -1 && \
    CONFIG_GPIO_RIGHT_BLINKER_SWITCH == -1 && \
    CONFIG_GPIO_MAIN_LIGHT_SWITCH == -1 && \
    CONFIG_GPIO_HAZARD_SWITCH == -1
#error No switches defined
#endif

estd::tuple<
    Debouncer<CONFIG_GPIO_LEFT_BLINKER_SWITCH>,
    Debouncer<CONFIG_GPIO_RIGHT_BLINKER_SWITCH>,
    Debouncer<CONFIG_GPIO_MAIN_LIGHT_SWITCH>,
    Debouncer<CONFIG_GPIO_HAZARD_SWITCH>
    > switches;

arduino_ostream cout(Serial);

#ifdef AUTOWP_LIB
static transport t(10);     // CS pin
#else
static transport t;
#endif


#if CONFIG_DIAGNOSTIC_CA
diagnostic_ca<transport, arduino_ostream> dca(cout);
#endif

ArduinoLightingCommandSink ca;

using States = embr::debounce::v1::States;
using clock = estd::chrono::arduino_clock;


// next scheduled debounce check
static clock::time_point next;


void setup()
{
    Serial.begin(115200);

#if CONFIG_SERIAL_WAIT
    while(!Serial);
#endif

    init_can(t);

    next = clock::now();

    cout << F("OEL source") << estd::endl;

    pinMode(CONFIG_GPIO_LEFT_BLINKER_SWITCH, INPUT);
    pinMode(CONFIG_GPIO_RIGHT_BLINKER_SWITCH, INPUT);
    pinMode(CONFIG_GPIO_MAIN_LIGHT_SWITCH, INPUT);
    pinMode(CONFIG_GPIO_HAZARD_SWITCH, INPUT);

    pinMode(CONFIG_GPIO_LEFT_BLINKER_INDICATOR, OUTPUT);
    pinMode(CONFIG_GPIO_RIGHT_BLINKER_INDICATOR, OUTPUT);
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

struct Visitor
{
    estd::bitset<SWITCH_MAX> changed;

    template <unsigned I>
    void operator()(embr::arduino::debounce::v1::ultimate::Event<I> e)
    {

    }
};

void loop()
{
    using traits = transport_traits<transport>;

    pdu<pgns::oel> pdu;

    transport::frame f;

    if(t.receive(&f))
    {
#if CONFIG_DIAGNOSTIC_CA
        process_incoming(dca, t, f);
#endif
        process_incoming(ca, t, f);
    }

    // if we haven't yet reached timeout to check debounce status, abort
    // and re-loop
    if(clock::now() < next) return;

    next += 10ms;

    bool v = false;
    Visitor v2;

    switches.visit(embr::arduino::debounce::v1::ultimate::Visitor{},
        [&](embr::debounce::v1::Event)
    {
        v = true;
    });
        // FIX: Odd, can't dedeuce template I
        //v2);

    /*
    bool v = get<SWITCH_LEFT_TURN>(switches).eval();
    
    v |= get<SWITCH_RIGHT_TURN>(switches).eval();
    v |= get<SWITCH_MAIN>(switches).eval(); */

    if(v)
    {
        using et = enum_type<spns::turn_signal_switch>;
        using et2 = enum_type<spns::main_light_switch>;

        States left_state = get<SWITCH_LEFT_TURN>(switches).state();
        States right_state = get<SWITCH_RIGHT_TURN>(switches).state();
        States main_state = get<SWITCH_MAIN>(switches).state();

        if(left_state == States::On && right_state == States::On)
        {
            // UNDEFINED, though might be good for hazard lights?
        }

        if(left_state == States::On)
            pdu.turn_signal_switch(et::left_turn_to_be_flashing);
        else if(right_state == States::On)
            pdu.turn_signal_switch(et::right_turn_to_be_flashing);

        pdu.main_light_switch(main_state == States::On ?
            et2::headlight_on : et2::off);

        // TODO
        pdu.hazard_light_switch(spn::measured::off);

        traits::send(t, pdu);
    }
}