#pragma once

#include "enum.h"
#include "slots.h"
#include <embr/units/base.h>

namespace embr { namespace j1939 {

// DEBT: 'void' tag because this is a kind of generic unit, theoretically fluidly
// convertible to other units.  Not sure how that should work though

template <>
struct slot_traits<slots::SAEct04>
{
    using type = embr::units::detail::unit<uint16_t, estd::ratio<1>, void>;
};

// UNTESTED
template <>
struct slot_traits<slots::SAEct09>
{
    using h = slot_traits_helper<int16_t, -125>;
    using type = embr::units::detail::unit<uint8_t, h::period, void, h::offset>;
};

}}