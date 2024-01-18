#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct celsius_tag {}; }

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using celsius = internal::unit_base<Rep, Period, internal::celsius_tag, F>;

template <class Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using centigrade = celsius<Rep, Period, F>;

template <>
struct traits<internal::celsius_tag>
{
    static constexpr const char* name() { return "degrees Celsius"; }

    // DEBT: Feature flag in proper degree symbol if we can.  Quick try of
    // https://stackoverflow.com/questions/23777226/how-to-display-degree-celsius-in-a-string-in-c
    // doesn't come through in esp.py terminal
    static constexpr const char* abbrev() { return " deg C"; }

    static constexpr si::quantities quanitiy = si::quantities::temperature;
};

inline namespace literals {

constexpr celsius<unsigned> operator ""_celsius(unsigned long long int v)
{
    return celsius<unsigned>(v);
}

}


}}