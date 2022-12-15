#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct pascals_tag {}; }

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using pascals = internal::unit_base<Rep, Period, internal::pascals_tag, F>;

}}