/**
 * References:
 *
 * 1. J1939-71 (MAR2011)
 */
#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::brake_switch> : internal::measured_type_traits {};

template<>
constexpr descriptor get_descriptor<spns::brake_switch>()
{
    return descriptor{ 4, 5, 2};
}

template<>
constexpr descriptor get_descriptor<spns::wheel_based_vehicle_speed>()
{
    return descriptor{ 2, 1, 16};
}

}

template <class TContainer>
struct data_field<pgns::ccvs, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    EMBR_J1939_PROPERTY(brake_switch)
};

}}