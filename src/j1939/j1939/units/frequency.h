#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct frequency_tag {}; }

template <>
struct traits<internal::frequency_tag>
{
    static constexpr const char* name() { return "hertz"; }
    static constexpr const char* abbrev() { return "Hz"; }
};

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using hz = internal::unit_base<Rep, Period, internal::frequency_tag, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using khz = hz<Rep, estd::ratio<1000, 1>>;

}}