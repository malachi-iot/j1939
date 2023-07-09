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
    EMBR_J1939_PGN_TRAIT("EJM1", "Extended Joystick Message 1")

    using spns = internal::spns_list<
        s::joystick1_theta_axis_clockwise_positive_position_status,
        s::joystick1_theta_axis_position>;
};

}

}}