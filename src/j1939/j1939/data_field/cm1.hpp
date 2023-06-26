#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::requested_percent_fan_speed> : internal::measured_type_traits {};

template <>
struct type_traits<spns::cab_interior_temperature_command> : internal::measured_type_traits {};

}


template<class TContainer>
struct data_field<pgns::cab_message1, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    data_field(const uint8_t* copy_from) : base_type(copy_from) {}
};


}}
