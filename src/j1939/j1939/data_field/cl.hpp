#pragma once

#include "base.hpp"

#include "../slots.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::illumination_brightness_percent> :
    internal::slot_type_traits<slots::SAEpc03> {};

template <>
struct type_traits<spns::switch_backlight_brightness_percent> :
    internal::slot_type_traits<slots::SAEpc03> {};


template<>
constexpr descriptor get_descriptor<spns::illumination_brightness_percent>()
{
    return { 1, 1, 8 };
}


template<>
constexpr descriptor get_descriptor<spns::switch_backlight_brightness_percent>()
{
    return { 2, 1, 8 };
}

}

namespace pgn {

template <>
struct traits<pgns::cab_illumination_message> : internal::traits_base
{
    EMBR_J1939_PGN_TRAIT("CL", "Cab Illumination Message 1")

    using spns = internal::spns_list<
        s::illumination_brightness_percent,
        s::switch_backlight_brightness_percent>;
};


}


template <class TContainer>
struct data_field<pgns::cab_illumination_message, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)
};


}}