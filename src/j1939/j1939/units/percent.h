#pragma once

#include <estd/internal/units/percent.h>

namespace embr { namespace units {

// Represented as 0 through 100, not 0 through 1
template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using percent = estd::internal::units::percent<Rep, Period, F>;

}}


