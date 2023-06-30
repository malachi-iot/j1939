#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct rpm_tag {}; }

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using rpm = internal::unit_base<Rep, Period, internal::rpm_tag, F>;

template <>
struct traits<internal::rpm_tag>
{
    static constexpr const char* name() { return "revolutions per minute"; }

    static constexpr const char* abbrev() { return "rpm"; }
};


}}