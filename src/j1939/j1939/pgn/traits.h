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



template <>
struct traits<pgns::lighting_data>
{
    static constexpr const char* name()
    {
        return "Lighting Data";
    }

    static constexpr const char* description()
    {
        return "A response to the request for lighting data in the lighting command message.";
    }

    static constexpr const char* abbrev() { return "LD"; }
};


template <>
struct traits<pgns::lighting_command>
{
    using s = embr::j1939::spns;

    using spns = internal::spns_list<
        s::alternate_headlights_cmd,
        s::low_beam_headlight_cmd,
        s::high_beam_headlight_cmd,
        s::right_turn_signal_lights_cmd,
        s::left_turn_signal_lights_cmd,
        s::backup_lights_and_alarm_horn_cmd,
        s::center_stop_lights_cmd,
        s::right_stop_lights_cmd,
        s::left_stop_lights_cmd,
        s::rear_fog_lights_cmd
    >;

    static constexpr const char* name()
    {
        return "Lighting Command";
    }

    static constexpr const char* description()
    {
        return "Message from the tractor to all lighting controllers on the tractor and attached implements";
    }

    static constexpr const char* abbrev() { return "LCMD"; }
};


}}}