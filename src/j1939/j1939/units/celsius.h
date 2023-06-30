#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct celsius_tag {}; }

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using celsius = internal::unit_base<Rep, Period, internal::celsius_tag, F>;

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using centigrade = celsius<Rep, Period, F>;

template <>
struct traits<internal::celsius_tag>
{
    static constexpr const char* name() { return "Celsius"; }

    // DEBT: Feature flag in proper degree symbol if we can
    static constexpr const char* abbrev() { return "deg C"; }

    static constexpr si::quantities quanitiy = si::quantities::temperature;
};


}}