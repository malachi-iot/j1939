#pragma once

#include "fwd.h"

namespace embr { namespace units {

// Copying the playbook from chrono
namespace internal {

// DEBT: Pretty sure theres a std/estd flavor of this we can use
template <typename TInt, TInt add>
struct adder
{
    typedef TInt int_type;
    static constexpr TInt value() { return add; }

    template <typename TInt2>
    constexpr TInt operator()(TInt2 v) const
    {
        return v + add;
    }
};



template <class ToScalar, class FromScalar>
constexpr ToScalar unit_cast(const FromScalar& s)
{
    typedef typename ToScalar::rep rep;
    typedef typename ToScalar::period period;

    return cast_helper<rep, period>::do_cast(s);
}

template <class Tag1, class Tag2>
struct compound_tag
{
    using tag1_type = Tag1;
    using tag2_type = Tag2;
};

// EXPERIMENTAL
// Playing with a specialization-generated rate/compound unit (aka mph, kph, mAh, etc)
template <class TUnit1, class TUnit2,
    typename Rep = typename estd::promoted_type<
        typename TUnit1::rep,
        typename TUnit2::rep>::type >
struct compound_unit_helper
{
    using tag_type = compound_tag<
        typename TUnit1::tag_type,
        typename TUnit2::tag_type>;

    // FIX: Need to grab rep/period from somewhere.  Really this ought to be deduced
    // by looking at the lcd/gcd characteristics of both rep/period from both types
    using rep = Rep;
    using period = typename TUnit1::period;

    // TODO: Wrestle with f/offset-er later
    using type = unit_base<rep, period, tag_type>;
};


// DEBT: Consolidate this with chrono if we can.  Specifically, I don't want disperate
// scalar bases intermingling with one another, so we need some kind of type lockout/forced
// conversion
///
/// @tparam Rep core unit size of underlying count/ticks
/// @tparam Period scaling ratio
/// @tparam Tag differentiating tag so as to disallow one unit from automatically converting to another
/// @tparam F final conversion.  defaults to passhtrough (noop)
template <typename Rep, class Period, class Tag, class F>
class unit_base : public Tag        // Deriving from tag not necessary, but might be useful for is_base_of query
{
protected:
    Rep rep_;

    // DEBT: Copy/pasted from estd::chrono duration
    // It does work, but it is prone to overflow so be careful.  Also,
    // chrono one is supposed to offer compile time protection against overflow
    // and it doesn't, so that's debt/FIX too
    template <class Rep2, class Period2, class F2>
    static constexpr Rep convert_from(const unit_base<Rep2, Period2, Tag, F2>& s)
    {
        typedef estd::ratio_divide<Period2, Period> rd;

        return s.count() * rd::num / rd::den;
    }

public:
    constexpr unit_base(const Rep& rep_) : rep_{rep_} {}

    template <class Rep2, class Period2, class F2>
    constexpr unit_base(const unit_base<Rep2, Period2, Tag, F2>& s) : rep_{convert_from(s)}
    {
    }

    typedef decltype(F{}(std::declval<Rep>())) rep;
    typedef Rep root_rep;
    //typedef Rep rep;

    typedef Period period;
    typedef Tag tag_type;

    // DEBT: Clean up name here, and document difference between so-called 'root' and regular
    constexpr root_rep root_count() const { return rep_; }

    constexpr rep count() const { return F{}(rep_); }

    // EXPERIMENTAL
    template <class TCompountUnit>
    using per = typename compound_unit_helper<unit_base, TCompountUnit>::type;
};


}}}