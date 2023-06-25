#pragma once

#include "slots.h"
#include "../units/percent.h"

namespace embr { namespace j1939 {

template <>
struct slot_traits<slots::SAEpc03>
{
    using type = embr::units::percent<uint8_t, estd::ratio<1, 250> >;
};


template <>
struct slot_traits<slots::SAEpc04>
{
    using h = slot_traits_helper<int, -100, estd::ratio<1, 125>>;
    using type = embr::units::percent<uint8_t, h::period, h::offset>;
};


}}
