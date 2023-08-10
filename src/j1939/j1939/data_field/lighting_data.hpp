/**
 * References:
 *
 * 1. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 */
#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::left_turn_signal_lights> : internal::measured_type_traits {};

template <>
struct type_traits<spns::right_turn_signal_lights> : internal::measured_type_traits {};

template <>
struct type_traits<spns::high_beam_headlight_data> : internal::measured_type_traits {};

template <>
struct type_traits<spns::low_beam_headlight_data> : internal::measured_type_traits {};

template <>
struct type_traits<spns::left_stop_lights> : internal::measured_type_traits {};

template <>
struct type_traits<spns::right_stop_lights> : internal::measured_type_traits {};

template <>
struct type_traits<spns::center_stop_lights> : internal::measured_type_traits {};

//template <>
//constexpr descriptor get_descriptor<spns::left_turn_lamp_circuit>

template <>
constexpr descriptor get_descriptor<spns::right_turn_signal_lights>()
{
    return descriptor{ 2, 5, 2};
}

template <>
constexpr descriptor get_descriptor<spns::left_turn_signal_lights>()
{
    return descriptor{ 2, 7, 2};
}

template <>
constexpr descriptor get_descriptor<spns::center_stop_lights>()
{
    return { 3, 3, 2 };
}

}

template <class TContainer>
struct data_field<pgns::lighting_data, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    EMBR_J1939_PROPERTY(running_light);
    
    EMBR_J1939_PROPERTY_ALIAS(left_turn_signal_lights, left_turn_signal);
    EMBR_J1939_PROPERTY_ALIAS(right_turn_signal_lights, right_turn_signal);
    EMBR_J1939_PROPERTY_ALIAS(center_stop_lights, center_stop);
};

namespace pgn {

template <>
struct traits<pgns::lighting_data> : internal::traits_base
{
    using spns = internal::spns_list<
        s::running_light,
        s::low_beam_headlight_data,
        s::high_beam_headlight_data,
        s::right_turn_signal_lights,
        s::left_turn_signal_lights,
        s::center_stop_lights,
        s::right_stop_lights,
        s::left_stop_lights,
        s::right_stop_lights>;

    static constexpr unsigned priority = 6;

    static constexpr const char* name()
    {
        return "Lighting Data";
    }

    static constexpr const char* description()
    {
        return "A response to the request for lighting data in the lighting command message.";
    }

    static constexpr const char* abbrev() { return "LD"; }
};


}

}}