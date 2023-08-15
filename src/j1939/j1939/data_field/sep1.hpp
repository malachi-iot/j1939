#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::sensor_supply_voltage_1> :
    internal::slot_type_traits<slots::SAEev01> {};

template <>
struct type_traits<spns::sensor_supply_voltage_2> :
    internal::slot_type_traits<slots::SAEev01> {};

template <>
struct type_traits<spns::sensor_supply_voltage_3> :
    internal::slot_type_traits<slots::SAEev01> {};

template <>
struct type_traits<spns::sensor_supply_voltage_4> :
    internal::slot_type_traits<slots::SAEev01> {};

template<>
constexpr descriptor get_descriptor<spns::sensor_supply_voltage_1>()
{
    return { 1, 1, 16 };
}

template<>
constexpr descriptor get_descriptor<spns::sensor_supply_voltage_2>()
{
    return { 3, 1, 16 };
}

}

}}