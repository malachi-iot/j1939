#pragma once

#include "base.hpp"

#include "../slots/percent.hpp"
#include "../slots/temperature.hpp"

#include <estd/ostream.h>       // for put_unit's use of estd::dec (supposed to be in ios and would be nice to fwd in iosfwd)
#include "../units/ostream.h"   // for put_unit

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
    internal::measured_type_traits {};

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
};

#if FEATURE_EMBR_J1939_OSTREAM_FULL_CM1
namespace internal {

template <>
struct payload_put<pgns::cab_message1> : estd::internal::ostream_functor_tag
{
    const data_field<pgns::cab_message1>& payload;

    constexpr explicit payload_put(const data_field<pgns::cab_message1>& payload) :
        payload{payload} {}

    template <class Streambuf, class Base>
    void operator()(estd::detail::basic_ostream<Streambuf, Base>& out) const
    {
        // DEBT: Bring in newer estd to get float support here
        embr::units::percent<short> p = payload.requested_percent_fan_speed();
        embr::units::celsius<short> c = payload.cab_interior_temperature_command();

        out << "temp=" << put_unit(c) << ' ' << "fan=" << put_unit(p);
    }
};
#endif

}


}}
