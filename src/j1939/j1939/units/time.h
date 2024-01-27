/**
 * References:
 *
 * 1. Reserved
 * 2. https://en.cppreference.com/w/cpp/header/chrono
 */
// DEBT: We might be able to finagle estd::chrono into here,
// but it won't have the offset/adjuster magic
#pragma once

#include <embr/units/seconds.h>
#include <estd/chrono.h>

namespace embr { namespace units {

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using seconds = detail::unit<Rep, Period, internal::seconds_tag, F>;

template <class Rep, typename F = internal::passthrough<Rep> >
using minutes = seconds<Rep, estd::chrono::minutes::period, F >;

template <class Rep, typename F = internal::passthrough<Rep> >
using hours = seconds<Rep, estd::chrono::hours::period, F >;

template <class Rep, typename F = internal::passthrough<Rep> >
using milliseconds = seconds<Rep, estd::milli, F >;

template <class Rep, typename F = internal::passthrough<Rep> >
using microseconds = seconds<Rep, estd::micro, F >;

template <class Rep, typename F = internal::passthrough<Rep> >
using days = seconds<Rep, estd::chrono::days::period, F >;

// DEBT: Upgrade estd::chrono to include months and years
template <class Rep, typename F = internal::passthrough<Rep> >
using months = seconds<Rep, estd::ratio<2629746>, F >;

// years.  Ratio lifted from [2]
// DEBT: Put this ratio into estd::chrono (works out to 365.2425 days)
template <class Rep, typename F = internal::passthrough<Rep> >
using years = seconds<Rep, estd::ratio<31556952>, F >;

}}