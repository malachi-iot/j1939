#include <estd/iosfwd.h>
#include <estd/locale.h>

#include "fwd.h"

namespace embr { namespace units {

namespace internal {

template <class TUnit>
struct unit_put
{
    const TUnit unit;
    const bool abbrev;
};

}

// DEBT: Slightly horrifying kludge for 'double' support in ostream
template <class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
    estd::detail::basic_ostream<TStreambuf, TBase>& out,
    double v)
{
    auto v_ = (int64_t)v;
    auto v_dec = (int64_t)(v * 100) % 100;

    out << v_;
    out << '.';
    out << v_dec;

    return out;
}

template <class Tag, class Period, class TStreambuf, class TBase,
    estd::enable_if_t<
        estd::is_same<Period, estd::ratio<1>>::value, bool> = true>
void write_suffix(estd::detail::basic_ostream<TStreambuf, TBase>& out)
{
    out << traits<Tag>::name();
}

template <class Tag, class Period, class TStreambuf, class TBase,
    estd::enable_if_t<
        !estd::is_same<Period, estd::ratio<1>>::value, bool> = true>
void write_suffix(estd::detail::basic_ostream<TStreambuf, TBase>& out)
{
    out << si::traits<Period>::name() << traits<Tag>::name();
}

template <class Tag, class Period, class TStreambuf, class TBase,
    estd::enable_if_t<
        estd::is_same<Period, estd::ratio<1>>::value, bool> = true>
void write_suffix_abbrev(estd::detail::basic_ostream<TStreambuf, TBase>& out)
{
    out << traits<Tag>::abbrev();
}

template <class Tag, class Period, class TStreambuf, class TBase,
    estd::enable_if_t<
        !estd::is_same<Period, estd::ratio<1>>::value, bool> = true>
void write_suffix_abbrev(estd::detail::basic_ostream<TStreambuf, TBase>& out)
{
    out << si::traits<Period>::abbrev() << traits<Tag>::abbrev();
}

template <class Rep, class Period, class F, class Tag, class TStreambuf, class TBase>
void write(estd::detail::basic_ostream<TStreambuf, TBase>& out,
    const internal::unit_base<Rep, Period, Tag, F>& unit)
{
    out << unit.count() << ' ';
    write_suffix<Tag, Period>(out);
}

template <class Rep, class Period, class F, class Tag, class TStreambuf, class TBase>
void write_abbrev(estd::detail::basic_ostream<TStreambuf, TBase>& out,
    const internal::unit_base<Rep, Period, Tag, F>& unit, bool include_space = false)
{
    out << unit.count();
    if(include_space) out << ' ';
    write_suffix_abbrev<Tag, Period>(out);
}

template <class TUnit, class TStreambuf, class TBase>
void test(estd::detail::basic_ostream<TStreambuf, TBase>& out,
    internal::unit_put<TUnit> v)
{}

namespace internal {

// For ADL to pick this up, it has to live in
// same namespace as unit_put
template <class TUnit, class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
    estd::detail::basic_ostream<TStreambuf, TBase>& out,
    unit_put<TUnit> v)
{
    if(v.abbrev)
        write_abbrev(out, v.unit);
    else
        write(out, v.unit);

    return out;
}

}



}

template <class Rep, class Period, class Tag, class F>
units::internal::unit_put<
    units::internal::unit_base<Rep, Period, Tag, F> > put_unit(
    const units::internal::unit_base<Rep, Period, Tag, F>& unit, bool abbrev = true)
{
    return { unit, abbrev };
}

}