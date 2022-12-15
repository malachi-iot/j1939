#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct rpm_tag {}; }

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using rpm = internal::unit_base<Rep, Period, internal::rpm_tag, F>;

}}