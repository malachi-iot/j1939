/**
 * References:
 *
 * 1. J1939-71 (MAR2011)
 */
#pragma once

#include "base.hpp"
#include "../slots/time.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
constexpr descriptor get_descriptor<spns::seconds>()
{ return descriptor{1, 1, 8}; }


template <>
constexpr descriptor get_descriptor<spns::minutes>()
{ return descriptor{2, 1, 8}; }

template <>
constexpr descriptor get_descriptor<spns::hours>()
{ return descriptor{3, 1, 8}; }


template <>
constexpr descriptor get_descriptor<spns::day>()
{ return descriptor{5, 1, 8}; }

template <>
constexpr descriptor get_descriptor<spns::month>()
{ return descriptor{4, 1, 8}; }

template <>
constexpr descriptor get_descriptor<spns::year>()
{ return descriptor{6, 1, 8}; }

template <>
constexpr descriptor get_descriptor<spns::local_minute_offset>()
{ return descriptor{7, 1, 8}; }

template <>
constexpr descriptor get_descriptor<spns::local_hour_offset>()
{ return descriptor{8, 1, 8}; }



template <>
struct type_traits<spns::seconds> : internal::slot_type_traits<slots::SAEtm04>
{

};

template <>
struct type_traits<spns::minutes> : internal::slot_type_traits<slots::SAEtm08>
{

};

template <>
struct type_traits<spns::hours> : internal::slot_type_traits<slots::SAEtm11>
{

};


template <>
struct type_traits<spns::day> : internal::slot_type_traits<slots::SAEcd01>
{

};


template <>
struct type_traits<spns::month> : internal::slot_type_traits<slots::SAEcm01>
{

};

template <>
struct type_traits<spns::local_minute_offset> : internal::slot_type_traits<slots::SAEtm07>
{

};

template <>
struct type_traits<spns::local_hour_offset> : internal::slot_type_traits<slots::SAEtm10>
{
    typedef internal::slot_type_traits<slots::SAEtm10> base_type;
    using typename base_type::int_type; // raw int type

    // DEBT: Naming not ideal, perhaps location could be better too
    struct raw
    {
        static constexpr int_type local_time() { return 0xFA; }
        static constexpr int_type not_supported() { return 0xF9; }
        static constexpr int_type min() { return 0x66; }
        static constexpr int_type max() { return 0x94; }
    };

    // DEBT: Naming not ideal, perhaps location could be better too
    struct cooked
    {
        static constexpr int8_t min() { return raw::min() + spn::internal::get_offset<slots::SAEtm10>(); }
        static constexpr int8_t max() { return raw::max() + spn::internal::get_offset<slots::SAEtm10>();; }
    };
};

}

namespace pgn {

template <>
struct traits<pgns::time_date> : internal::traits_base
{
    EMBR_J1939_PGN_TRAIT("TD", "Time/Date");

    using spns = internal::spns_list<
        s::seconds,
        s::minutes,
        s::hours,
        s::month,
        s::day,
        s::year,
        s::local_minute_offset,
        s::local_hour_offset>;
};


template <>
struct traits<pgns::time_date_adjust> : internal::traits_base
{
    EMBR_J1939_PGN_TRAIT("TDA", "Time/Date Adjust");

    using spns = internal::spns_list<
        s::adjust_seconds,
        s::adjust_minutes,
        s::adjust_hours,
        s::adjust_month,
        s::adjust_year,
        s::adjust_local_minute_offset,
        s::adjust_local_hour_offset>;
};

}


template <class TContainer>
struct data_field<pgns::vehicle_hours, TContainer> : internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)
};


template <class TContainer>
struct data_field<pgns::time_date, TContainer> : internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    EMBR_J1939_PROPERTY(seconds);
    EMBR_J1939_PROPERTY(minutes);
    EMBR_J1939_PROPERTY(hours);
    EMBR_J1939_PROPERTY(day);
    EMBR_J1939_PROPERTY(month);
    EMBR_J1939_PROPERTY(year);
    EMBR_J1939_PROPERTY(local_minute_offset);
    EMBR_J1939_PROPERTY(local_hour_offset);
};


template <class TContainer>
struct data_field<pgns::time_date_adjust, TContainer> : internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)
};


namespace internal {

template <>
struct payload_put<pgns::time_date> : estd::internal::ostream_functor_tag
{
    const data_field<pgns::time_date>& payload;

    constexpr explicit payload_put(const data_field<pgns::time_date>& payload) : payload{payload} {}

    template <class TStreambuf, class TBase>
    void operator()(estd::detail::basic_ostream<TStreambuf, TBase>& out) const
    {
        // DEBT: We'd prefer not to have to reach into count() here
        out << payload.minutes().count() << ':' << payload.seconds().count();
    }
};

}


}}