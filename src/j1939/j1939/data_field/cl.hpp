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
    static constexpr const char* name() { return "Cab Illumination Message"; }
    static constexpr const char* abbrev() { return "CL"; }

    using spns = internal::spns_list<
        s::illumination_brightness_percent,
        s::switch_backlight_brightness_percent>;
};


}

}}