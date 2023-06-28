#pragma once

#include "base.hpp"
#include "../slots/percent.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::estimated_percent_fan_speed> :
    internal::slot_type_traits<slots::SAEpc03> {};

template <>
struct type_traits<spns::fan_drive_state> :
    internal::type_traits_base<uint8_t>
{
    enum class enum_type : uint8_t
    {
        off = 0,
        engine_system_general,
        excessive_engine_air_temperature,
        excessive_engine_oil_temperature,
        excessive_engine_coolant_temperature,
        excessive_engine_transmission_oil_temperature,
        excessive_engine_hydraulic_oil_temperature,
        default_operation,
        not_defined,
        manual_control,
        transmission_retarder,
        ac_system,
        timer,
        engine_brake,
        other,
        not_available,
        noop = not_available
    };

    using value_type = enum_type;
};


template<>
constexpr descriptor get_descriptor<spns::estimated_percent_fan_speed>()
{
    return { 1, 1, 8 };
}

template<>
constexpr descriptor get_descriptor<spns::fan_drive_state>()
{
    return { 2, 1, 4 };
}

template<>
constexpr descriptor get_descriptor<spns::fan_speed>()
{
    return { 3, 1, 16 };
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
    EMBR_J1939_PROPERTY(fan_drive_state);
};

}}
