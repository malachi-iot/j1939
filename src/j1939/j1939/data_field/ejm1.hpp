#pragma once

#include "base.hpp"

#include "../slots.hpp"

namespace embr { namespace j1939 {

namespace spn {

}

namespace pgn {

template <>
struct traits<pgns::extended_joystick_message_1> : internal::traits_base
{
    static constexpr const char* name = "Extended Joystick Message 1";
    static constexpr const char* abbrev = "EJM1";

    using spns = internal::spns_list<
        s::joystick1_theta_axis_clockwise_positive_position_status,
        s::joystick1_theta_axis_position>;
};

}

}}