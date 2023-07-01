#pragma once

#include "base.h"
#include "time.h"

namespace embr { namespace units {

namespace internal {

struct feet_tag {};
struct meters_tag {};

using foot_second_tag = compound_tag<feet_tag, seconds_tag>;
using meters_second_tag = compound_tag<meters_tag, seconds_tag>;

}

template <typename Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using meters = internal::unit_base<Rep, Period, internal::meters_tag, F>;

template <typename Rep, typename F = internal::passthrough<Rep> >
using centimeters = meters<Rep, estd::centi, F>;

template <typename Rep, typename F = internal::passthrough<Rep> >
using millimeters = meters<Rep, estd::milli, F>;

template <typename Rep, typename F = internal::passthrough<Rep> >
using kilometers = meters<Rep, estd::kilo, F>;

template <typename Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using feet = internal::unit_base<Rep, Period, internal::feet_tag, F>;

template <typename Rep, typename F = internal::passthrough<Rep> >
using inches = feet<Rep, estd::ratio<1, 12>, F>;

template <typename Rep, typename F = internal::passthrough<Rep> >
using yards = feet<Rep, estd::ratio<3>, F>;

template <typename Rep, typename F = internal::passthrough<Rep> >
using miles = feet<Rep, estd::ratio<5280>, F>;

template <typename Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using meters_per_second = internal::unit_base<Rep, Period, internal::meters_second_tag, F>;

template <typename Rep, typename F = internal::passthrough<Rep> >
using kilometers_per_second = meters_per_second<Rep, estd::ratio<1, estd::kilo::den>, F>;

// DEBT: I think we can use a clever specializing technique to do something like
// units_rate_per<kilometers, hours>
template <typename Rep, typename F = internal::passthrough<Rep> >
using kilometers_per_hour = meters_per_second<Rep,
    estd::ratio<
        estd::kilo::num,
        estd::chrono::hours::period::num>,
    F>;

// NOTE: These compound types don't seem to be a great fit for these traits so far
template <>
struct traits<internal::meters_second_tag>
{
    static constexpr const char* name() { return "meters per second"; }
    static constexpr const char* abbrev() { return "m/s"; }
};


}}