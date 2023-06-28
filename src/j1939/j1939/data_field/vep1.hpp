/**
 * References:
 *
 * 1. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 */
#pragma once

#include "base.hpp"

#include "../units/volts.h"
#include "../slots.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
constexpr descriptor get_descriptor<spns::battery_potential>()
{
    return descriptor{3, 1, 16};
}

template <>
struct type_traits<spns::battery_potential> : internal::slot_type_traits<slots::SAEev01>
{
};

}

template <class TContainer>
struct data_field<pgns::vehicle_electrical_power_1, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    unit_type<spns::battery_potential> battery_potential() const
    {
        return unit_type<spns::battery_potential>(
            base_type::template get_raw<spns::battery_potential>());
    }

    void battery_potential(uint16_t v)
    {
        base_type::template set<spns::battery_potential>(v);
    }

    void battery_potential(const spn::traits<spns::battery_potential>::value_type& v)
    {
        battery_potential(v.count());
    }
};

}}