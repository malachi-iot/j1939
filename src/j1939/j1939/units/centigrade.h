#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct centigrade_tag {}; }

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using centigrade = internal::unit_base<Rep, Period, internal::centigrade_tag, F>;

}}