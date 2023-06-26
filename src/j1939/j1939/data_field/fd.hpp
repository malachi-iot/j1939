#pragma once

#include "base.hpp"
#include "../slots/percent.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::estimated_percent_fan_speed> :
    //internal::measured_type_traits {};
    internal::slot_type_traits<slots::SAEpc03> {};


template<>
constexpr descriptor get_descriptor<spns::estimated_percent_fan_speed>()
{
    return descriptor{ 1, 1, 1 };
}

}

template<class TContainer>
struct data_field<pgns::fan_drive_1, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    data_field(const uint8_t* copy_from) : base_type(copy_from) {}

    EMBR_J1939_PROPERTY(estimated_percent_fan_speed);
};

}}
