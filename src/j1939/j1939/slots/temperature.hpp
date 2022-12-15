#pragma once

#include "slots.h"
#include "../units/centigrade.h"

namespace embr { namespace j1939 {


template <>
struct slot_traits<slots::SAEtp01>
{
    // NOTE: max() is always pre ratio pre offset raw maximum.  This way we don't have to
    // worry about decimal places or signed types, and min() is always 0 (probably)
    static constexpr uint8_t max() { return 250; }

    using h = slot_traits_helper<int16_t, -40>;
    using type = embr::units::centigrade<uint8_t, h::period, h::offset>;
};


}}