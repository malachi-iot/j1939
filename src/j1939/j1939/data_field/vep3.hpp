// Vehicle Electric Power #3
#pragma once

#include "base.hpp"

#include <embr/units/amps.h>
#include "../slots.hpp"

#define EMBR_J1939_SPN_DECLARE_SAE(name, slot, bytepos, bitpos, bitwidth)  \
template <> constexpr descriptor get_descriptor<spns::name>()   \
{ descriptor{bytepos, bitpos, bitwidth}; }  \
template <> struct type_traits<spns::name> : \
    internal::slot_type_traits<slots::slot> \
{};



namespace embr { namespace j1939 {

namespace spn {

template <>
constexpr descriptor get_descriptor<spns::alternator_current_highres>()
{
    return descriptor{1, 1, 16};
}

template <>
constexpr descriptor get_descriptor<spns::net_battery_current_highres>()
{
    return descriptor{3, 1, 16};
}


template <>
struct type_traits<spns::alternator_current_highres> : internal::slot_type_traits<slots::SAEec01>
{
};

template <>
struct type_traits<spns::net_battery_current_highres> : internal::slot_type_traits<slots::SAEec01>
{
};


}

template <class TContainer>
struct data_field<pgns::vehicle_electrical_power_3, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    // DEBT: Rework ALIAS to auto-implement non-aliased also
    // Not doing so yet because of DEBT presently on EMBR_J1939_PROPERTY

    EMBR_J1939_PROPERTY_ALIAS(alternator_current_highres, alternator_current)
    EMBR_J1939_PROPERTY_ALIAS(net_battery_current_highres, net_battery_current)
    
    EMBR_J1939_PROPERTY(alternator_current_highres)
    EMBR_J1939_PROPERTY(net_battery_current_highres)
};

}}