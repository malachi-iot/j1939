#pragma once

#include "base.h"
#include "amps.h"
#include "volts.h"

namespace embr { namespace units {

namespace internal {

using watts_tag = compound_tag<volts_tag, amps_tag>;

}

template <typename Rep, class Period = estd::ratio<1>, typename F = internal::passthrough<Rep> >
using watts = internal::unit_base<Rep, Period, internal::watts_tag, F>;

template <typename Rep, typename F = internal::passthrough<Rep> >
using milliwatts = watts<Rep, estd::milli, F>;

// EXPERIMENTAL
template <typename Rep, class Period, typename Rep2, class Period2,
    class Period3 = estd::ratio_multiply<Period, Period2>>
// DEBT: estd::radio_multiply and estd::ratio_divide need to do a using = for c++11, otherwise we're
// forced to "reratio" it as seen here
constexpr amps<Rep, estd::ratio<Period3::num, Period3::den> > operator /(const watts<Rep, Period>& lhs, const volts<Rep2, Period2>& rhs)
{
    // TBD
    return 0;
}


}}