// Vehicle Electric Power #3
#pragma once

#include "base.hpp"

#include "../units/volts.h"
#include "../slots.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
constexpr descriptor get_descriptor<spns::alternator_current_highres>()
{
    return descriptor{1, 1, 16};
}

template <>
constexpr descriptor get_descriptor<spns::net_battery_current_highres>()
{
    return descriptor{3, 1, 16};
}


}

}}