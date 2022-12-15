#pragma once

#include "fwd.h"
#include "base.h"
#include "time.h"

// DEBT: Review the SAE j1939 standard units docs before going too far into this
// and other of our units/ folder friends
namespace embr { namespace units {

namespace internal {

struct amps_tag {};
using amp_seconds_tag = compound_tag<amps_tag, seconds_tag>;

}

///
/// @tparam Rep core unit size of underlying count/ticks
/// @tparam Period scaling ratio
/// @tparam F final conversion.  defaults to passhtrough (noop)
template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using amps = internal::unit_base<Rep, Period, internal::amps_tag, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using milliamps = amps<Rep, estd::milli, F>;

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using amp_seconds = internal::unit_base<Rep, Period, internal::amp_seconds_tag, F>;

// +++ EXPERIMENTAL, untested.  Might want a rate_unit_base, though I like the flexibility of
// basing it all on an underlying compound tag
template <class Rep, typename F = internal::passthrough<Rep> >
using milliamp_seconds = amp_seconds<Rep, estd::ratio<1000>, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using amp_hours = amp_seconds<Rep, estd::ratio<3600>, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using milliamp_hours = amp_seconds<Rep, estd::ratio<1000UL * 3600>, F>;
// --- EXPERIMENTAL

}}