#pragma once

#include "slots.h"
#include "../units/percent.h"

namespace embr { namespace j1939 {


template <>
struct slot_traits<slots::SAEpc02>
{
    using type = embr::units::percent<uint16_t, estd::deci>;
};

template <>
struct slot_traits<slots::SAEpc03>
{
    using type = embr::units::percent<uint8_t, estd::ratio<100, 250> >;
};


template <>
struct slot_traits<slots::SAEpc04>
{
    using h = slot_traits_helper<int8_t, -100, estd::ratio<100, 125>>;
    using type = h::unit<uint8_t, embr::units::percent>;
};


template <>
struct slot_traits<slots::SAEpc07>
{
    using h = slot_traits_helper<int16_t, -100, estd::deci>;
    using type = h::unit<uint16_t, embr::units::percent>;
};


template <>
struct slot_traits<slots::SAEpc08>
{
    using h = slot_traits_helper<int32_t, -251, estd::ratio<1, 128> >;
    using type = h::unit<uint16_t, embr::units::percent>;
};


}}
