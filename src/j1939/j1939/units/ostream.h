#include <estd/iosfwd.h>

#include "fwd.h"

namespace embr { namespace units {

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


}}