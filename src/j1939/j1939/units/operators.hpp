#pragma once

#include <estd/ratio.h>

#include <estd/internal/units/operators.hpp>

#include <embr/units/amps.h>

namespace embr { namespace units {

// EXPERIMENTAL
template <typename Rep, class Period, class Tag, class F,
    typename Rep2, class Period2, class F2,
    class Period3 = estd::ratio_multiply<Period, Period2>>
constexpr amps<Rep, Period3> operator /(
    const detail::unit<Rep, Period, Tag, F>& lhs,
    // DEBT: Tag::tag2 is a cheap and nasty callout to compound_tag.  We can do better
    const detail::unit<Rep2, Period2, typename Tag::tag2, F2>& rhs)
{
    typedef detail::unit<Rep, Period, Tag, F> lhs_type;
    typedef detail::unit<Rep2, Period2, typename Tag::tag2, F2> rhs_type;

    typedef typename estd::promoted_type<Rep,Rep2>::type p_type;

    p_type v = lhs.count() / rhs.count();
    // TBD
    return 0;
}

}}