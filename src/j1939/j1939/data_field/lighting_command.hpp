/**
 * References:
 *
 * 1. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 * 2. https://www.isobus.net/isobus/pGNAndSPN/12804?type=PGN
 * 3. J1939-71 (MAR2011)
 */
#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::left_stop_lights_cmd> : internal::status_type_traits {};

template <>
struct type_traits<spns::right_stop_lights_cmd> : internal::status_type_traits {};

template <>
struct type_traits<spns::center_stop_lights_cmd> : internal::status_type_traits {};

template <>
struct type_traits<spns::left_turn_signal_lights_cmd> : internal::status_type_traits {};

template <>
struct type_traits<spns::right_turn_signal_lights_cmd> : internal::status_type_traits {};

template <>
struct type_traits<spns::low_beam_headlight_cmd> : internal::status_type_traits {};

template <>
struct type_traits<spns::high_beam_headlight_cmd> : internal::status_type_traits {};

template <>
struct type_traits<spns::front_fog_lights_cmd> : internal::status_type_traits {};

template <>
struct type_traits<spns::lighting_data_message_request> : internal::type_traits_base<uint8_t>
{
    enum class enum_type
    {
        no_data_requested = 0,
        data_requested,
        reserved,
        noop
    };

    typedef enum_type value_type;
};


template<>
constexpr descriptor get_descriptor<spns::center_stop_lights_cmd>()
{
    return descriptor{ 3, 3, 2};
}

template<>
constexpr descriptor get_descriptor<spns::front_fog_lights_cmd>()
{
    return descriptor{ 2, 1, 2};
}


template<>
constexpr descriptor get_descriptor<spns::left_stop_lights_cmd>()
{
    return descriptor{ 3, 7, 2};
}


template<>
constexpr descriptor get_descriptor<spns::right_stop_lights_cmd>()
{
    return descriptor{ 3, 5, 2};
}

template<>
constexpr descriptor get_descriptor<spns::alternate_headlights_cmd>()
{
    return descriptor{ 1, 3, 2};
}

// linked to pgns::lighting_command
// [3? reference broken] p. 236
template <>
constexpr descriptor get_descriptor<spns::left_turn_signal_lights_cmd>()
{
    return descriptor{2, 7, 2};
};


// linked to pgns::lighting_command
// [3? reference broken] p. 236
template <>
constexpr descriptor get_descriptor<spns::right_turn_signal_lights_cmd>()
{
    return descriptor{2, 5, 2};
};

template <>
constexpr descriptor get_descriptor<spns::lighting_data_message_request>()
{
    return descriptor{8, 1, 2};
};

template <>
constexpr descriptor get_descriptor<spns::low_beam_headlight_cmd>()
{
    return descriptor{1, 5, 2};
};

template <>
constexpr descriptor get_descriptor<spns::high_beam_headlight_cmd>()
{
    return descriptor{1, 7, 2};
};

}

// Represents [reference broken] Section 5.2. generally and [1] Section 5.2.7.1. specifically
template<class TContainer>
struct data_field<pgns::lighting_command, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    data_field(const uint8_t* copy_from) : base_type(copy_from) {}

    EMBR_J1939_PROPERTY_ALIAS(front_fog_lights_cmd, front_fog_lights);
    EMBR_J1939_PROPERTY_ALIAS(low_beam_headlight_cmd, low_beam_headlight);
    EMBR_J1939_PROPERTY_ALIAS(high_beam_headlight_cmd, high_beam_headlight);
    EMBR_J1939_PROPERTY_ALIAS(left_turn_signal_lights_cmd, left_turn_signal);
    EMBR_J1939_PROPERTY_ALIAS(right_turn_signal_lights_cmd, right_turn_signal);
    EMBR_J1939_PROPERTY_ALIAS(lighting_data_message_request, request);
    EMBR_J1939_PROPERTY_ALIAS(center_stop_lights_cmd, center_stop);
    EMBR_J1939_PROPERTY_ALIAS(right_stop_lights_cmd, right_stop);
    EMBR_J1939_PROPERTY_ALIAS(left_stop_lights_cmd, left_stop);
};

namespace pgn {

template <>
struct traits<pgns::lighting_command>
{
    using s = embr::j1939::spns;

    using spns = internal::spns_list<
        s::alternate_headlights_cmd,
        s::low_beam_headlight_cmd,
        s::high_beam_headlight_cmd,
        s::right_turn_signal_lights_cmd,
        s::left_turn_signal_lights_cmd,
        s::backup_lights_and_alarm_horn_cmd,
        s::center_stop_lights_cmd,
        s::right_stop_lights_cmd,
        s::left_stop_lights_cmd,
        s::rear_fog_lights_cmd
    >;

    static constexpr const char* name()
    {
        return "Lighting Command";
    }

    static constexpr const char* description()
    {
        return "Message from the tractor to all lighting controllers on the tractor and attached implements";
    }

    static constexpr const char* abbrev() { return "LCMD"; }
};


}


}}