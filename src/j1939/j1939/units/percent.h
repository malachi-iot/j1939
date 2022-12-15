#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct percent_tag {}; }

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using percent = internal::unit_base<Rep, Period, internal::percent_tag, F>;

}}