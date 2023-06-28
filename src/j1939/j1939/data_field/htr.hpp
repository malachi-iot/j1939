#pragma once

#include "base.hpp"

#include "../spn/fwd.h"
#include "../spn/traits.h"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::auxiliary_heater_water_pump_status> {};

template <>
struct type_traits<spns::cab_ventilation> {};

template <>
struct type_traits<spns::cab_heating_zone> {};


template <>
constexpr descriptor get_descriptor<spns::cab_ventilation>()
{
    return { 5, 3, 2 };
}

template <>
constexpr descriptor get_descriptor<spns::cab_heating_zone>()
{
    return { 5, 7, 2 };
}


}

}}
