#pragma once

#include <embr/units/celsius.h>

#include "slots.h"

namespace embr { namespace j1939 {


template <>
struct slot_traits<slots::SAEtp01>
{
    // NOTE: max() is always pre ratio pre offset raw maximum.  This way we don't have to
    // worry about decimal places or signed types, and min() is always 0 (probably)
    static constexpr uint8_t max() { return 250; }

    using h = slot_traits_helper<int16_t, -40>;
    using type = embr::units::celsius<uint8_t, h::period, h::offset>;
};

// Not yet tested
template <>
struct slot_traits<slots::SAEtp02>
{
    using h = slot_traits_helper<int16_t, -273, estd::ratio<3125, 100000>>;
    using type = h::unit<uint16_t, units::celsius>;
};


}}