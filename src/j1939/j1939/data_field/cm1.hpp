#pragma once

#include "base.hpp"

#include "../slots/percent.hpp"
#include "../slots/temperature.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::requested_percent_fan_speed> :
    internal::slot_type_traits<slots::SAEpc03> {};

template <>
struct type_traits<spns::cab_interior_temperature_command> :
    internal::slot_type_traits<slots::SAEtp02> {};

template<>
constexpr descriptor get_descriptor<spns::requested_percent_fan_speed>()
{
    return { 1, 1, 8 };
}

template<>
constexpr descriptor get_descriptor<spns::cab_interior_temperature_command>()
{
    return { 2, 1, 16 };
}

}


template<class TContainer>
struct data_field<pgns::cab_message1, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    constexpr data_field(const uint8_t* copy_from) : base_type(copy_from) {}

    EMBR_J1939_PROPERTY(requested_percent_fan_speed);
    EMBR_J1939_PROPERTY(cab_interior_temperature_command);
};


}}
