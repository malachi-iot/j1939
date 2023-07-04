#pragma once

#include <estd/internal/variadic.h>

#include "fwd.h"
#include "enum.h"

namespace embr { namespace j1939 { namespace pgn {

#if __cpp_concepts
#endif

namespace internal {

template <spns ...Values>
using spns_list = estd::variadic::values<spns, Values...>;

}


template <>
struct traits<pgns::oel>
{
    using s = embr::j1939::spns;
    
    using spns = internal::spns_list<
        s::turn_signal_switch,
        s::high_low_beam_switch,
        s::work_light_switch,
        s::main_light_switch,
        s::hazard_light_switch,
        s::operators_desired_delay_lamp_off_time>;

    static constexpr const char* name()
    {
        return "Operator External Light Controls";
    }

    static constexpr const char* abbrev() { return "OEL"; }
};


template <>
struct traits<pgns::fan_drive_1>
{
    static constexpr const char* name()
    {
        return "Fan Drive 1";
    }

    static constexpr const char* abbrev() { return "FD1"; }
};


template <>
struct traits<pgns::cab_message1>
{
    using s = embr::j1939::spns;

    using spns = internal::spns_list<
        s::requested_percent_fan_speed,
        s::cab_interior_temperature_command>;

    static constexpr const char* name()
    {
        return "Cab Message 1";
    }

    static constexpr const char* description()
    {
        return "Message containing parameters originating from the vehicle cab.";
    }

    static constexpr const char* abbrev() { return "CM1"; }
};


template <>
struct traits<pgns::cab_message_3>
{
    static constexpr const char* name()
    {
        return "Cab Message 3";
    }

    static constexpr const char* description()
    {
        return "Provides information from Cab mounted operator inputs.";
    }

    static constexpr const char* abbrev() { return "CM3"; }
};


}}}