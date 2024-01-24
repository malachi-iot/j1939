#pragma once

#include "base.hpp"

#include "../slots/percent.hpp"
#include "../slots/temperature.hpp"

#include <estd/ostream.h>       // for put_unit's use of estd::dec (supposed to be in ios and would be nice to fwd in iosfwd)
#include "../units/ostream.h"   // for put_unit
#include "../slots/units.h"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::requested_percent_fan_speed> :
    internal::slot_type_traits<slots::SAEpc03> {};

template <>
struct type_traits<spns::cab_interior_temperature_command> :
    internal::slot_type_traits<slots::SAEtp02> {};

template <>
struct type_traits<spns::battery_main_switch_hold_request> :
    internal::status_type_traits {};

template <>
struct type_traits<spns::request_cab_zone_heating> :
    internal::status_type_traits {};

template <>
struct type_traits<spns::request_engine_zone_heating> :
    internal::status_type_traits {};

template<>
constexpr descriptor get_descriptor<spns::requested_percent_fan_speed>()
{
    return { 1, 1, 8 };
}

template<>
constexpr descriptor get_descriptor<spns::cab_interior_temperature_command>()
{
    return { 2, 1, 16 };
}

template<>
constexpr descriptor get_descriptor<spns::request_cab_zone_heating>()
{
    return { 7, 7, 2 };
}


}


template<class TContainer>
struct data_field<pgns::cab_message1, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    constexpr data_field(const uint8_t* copy_from) : base_type(copy_from) {}

    EMBR_J1939_PROPERTY(requested_percent_fan_speed);
    EMBR_J1939_PROPERTY(cab_interior_temperature_command);
    EMBR_J1939_PROPERTY(request_cab_zone_heating);

    // EXPERIMENTAL, improved unit_type with lots more compile-time info
    // about SPN and associated slot
    spn::unit<spns::cab_interior_temperature_command> test() const
    {
        return base_type::template get<spns::cab_interior_temperature_command>();
    }
};

#if FEATURE_EMBR_J1939_OSTREAM_FULL_CM1
namespace internal {

// EXPERIMENTAL
template <spns spn_, class Coerce, class Streambuf, class Base>
estd::detail::basic_ostream<Streambuf, Base>& put_unit_j1939(
    estd::detail::basic_ostream<Streambuf, Base>& out,
    typename spn::traits<spn_>::value_type v)
{
    if(spn::traits<spn_>::noop(v))
        out << "noop";
    else
        out << put_unit(Coerce(v));

    return out;
}


template <>
struct payload_put<pgns::cab_message1> : estd::internal::ostream_functor_tag
{
    const data_field<pgns::cab_message1>& payload;

    constexpr explicit payload_put(const data_field<pgns::cab_message1>& payload) :
        payload{payload} {}

    template <class Streambuf, class Base>
    void operator()(estd::detail::basic_ostream<Streambuf, Base>& out) const
    {
#if FEATURE_EMBR_J1939_OSTREAM_FLOAT
        // DEBT: Upgrade estd to get real float support here.  embr::units has a pretty
        // gnarly kludge to do this
        using precision = double;
#else
        using precision = short;
#endif

        // DEBT: Filter out further by null/noop values, and make it less verbose
        // would be nice to wrap embr::units with a embr::j1939::units (or similar)
        // which brought along spn::traits, convvenience noop, and others, for the ride
        using c_traits = spn::traits<spns::cab_interior_temperature_command>;
        const c_traits::value_type c(payload.cab_interior_temperature_command());

        out << estd::dec;   // DEBT: Needing to be way too explicit about this guy
        out << "temp=";

        if(c_traits::noop(c))
            out << "noop";
        else
            out << put_unit(units::celsius<precision>(c));

        out << ' ' << "fan=";

        put_unit_j1939<
            spns::requested_percent_fan_speed,
            units::percent<precision> >(out, payload.requested_percent_fan_speed());
    }
};
#endif

}


}}
