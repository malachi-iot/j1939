#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct percent_tag {}; }

// Represented as 0 through 100, not 0 through 1
template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using percent = internal::unit_base<Rep, Period, internal::percent_tag, F>;

inline namespace literals {

constexpr percent<unsigned> operator ""_pct (unsigned long long int v)
{
    return percent<unsigned>(v);
}

constexpr percent<double> operator ""_pct (long double v)
{
    return percent<double>(static_cast<double>(v));
}

}


template <>
struct traits<internal::percent_tag>
{
    static constexpr const char* name() { return "percent"; }
    // FIX: Under esp-idf, this gets consumed somehow.  Does not need escaping
    // under linux.
    static constexpr const char* abbrev() { return "%"; }
};

//inline namespace v1 {

namespace internal {

// These feel not quite ready yet - i.e. what's the major upgrade from a
// special multiply?  we keep coming back to the oddball 0-100 behavior of percent
template <class Rep1, class Ratio1, class F1,
    class Rep2, class Ratio2, class Tag, class F2>
inline internal::unit_base<Rep2, Ratio2, Tag, F2> get_from_percent(
    const percent<Rep1, Ratio1, F1>& p,
    internal::unit_base<Rep2, Ratio2, Tag, F2> min,
    internal::unit_base<Rep2, Ratio2, Tag, F2> max)
{
    using target_type = internal::unit_base<Rep2, Ratio2, Tag, F2>;
    const target_type range = max - min;

    // DEBT: Use common_type here
    auto v = range.count() * p.count();

    // DEBT: Perhaps inspect num vs 100 to see if we would compile-time
    // calc differently - though I bet compiler will do a handy job of this
    // already
    v *= Ratio1::num;
    v /= 100 * Ratio1::den;

    return target_type(v + min.count());
}

template <class Rep1, class Ratio1, class F1,
    class Rep2, class Ratio2, class Tag, class F2>
inline internal::unit_base<Rep2, Ratio2, Tag, F2> get_from_percent(
    const percent<Rep1, Ratio1, F1>& p,
    internal::unit_base<Rep2, Ratio2, Tag, F2> max)
{
    return get_from_percent(p, internal::unit_base<Rep2, Ratio2, Tag, F2>(0), max);
}


}


}}
