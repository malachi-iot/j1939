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
    static constexpr const char* abbrev() { return "%"; }
};


}}
