#pragma once

#include "fwd.h"
#include "base.h"

// DEBT: Review the SAE j1939 standard units docs before going too far into this
// and other of our units/ folder friends
namespace embr { namespace units {

namespace internal {

struct volts_tag {};

}

///
/// @tparam Rep core unit size of underlying count/ticks
/// @tparam Period scaling ratio
/// @tparam F final conversion.  defaults to passhtrough (noop)
template <typename Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using volts = internal::unit_base<Rep, Period, internal::volts_tag, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using kilovolts = volts<Rep, estd::kilo, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using decivolts = volts<Rep, estd::deci, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using millivolts = volts<Rep, estd::milli, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using microvolts = volts<Rep, estd::micro, F>;



}}