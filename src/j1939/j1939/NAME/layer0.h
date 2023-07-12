#pragma once

#include "fwd.h"
#include "function.h"
#include "manufacturer.h"
#include "industry_groups.h"
#include "vehicle_systems.h"

namespace embr { namespace j1939 { namespace layer0 {

template <bool arbitrary_address_, industry_groups ig_,
    vehicle_systems vs_,
    function_fields f_,
    int32_t identity_number_ = -1,
    manufacturer_codes mc_ = manufacturer_codes::reserved>
struct NAME
{
    static constexpr bool arbitrary_address_capable = arbitrary_address_;
    static constexpr industry_groups industry_group = ig_;
    static constexpr vehicle_systems vehicle_system = vs_;
    static constexpr function_fields function = f_;
    static constexpr manufacturer_codes manufacturer_code = mc_;
    // -1 means none specified, and anything will match
    // DEBT: Make this something like a 22 bit signed word, though really it's not needed
    static constexpr int32_t identity_number = identity_number_;

    // NOTE: Does NOT match configurable fields such as instances
    template <class TContainer>
    constexpr static bool match(const j1939::NAME<TContainer>& n)
    {
        return
            n.arbitrary_address_capable() == arbitrary_address_capable &&
            vehicle_system == (vehicle_systems)n.vehicle_system() &&
                (manufacturer_code == manufacturer_codes::not_applicable ||
                manufacturer_code == (manufacturer_codes)n.manufacturer_code().value()) &&
                (identity_number == -1 || identity_number == n.identity_number().value()) &&
            function == (function_fields)n.function().value();
    }

    template <class TContainer>
    static void populate(j1939::NAME<TContainer>& n,
        word<4> vehicle_system_instance = {0},
        word<5> function_instance = {0},
        word<3> ecu_instance = {0})
    {
        n.arbitrary_address_capable(arbitrary_address_capable);
        n.vehicle_system((uint8_t)vehicle_system);

        if(manufacturer_code != manufacturer_codes::not_applicable)
            n.manufacturer_code((int16_t)manufacturer_code);

        n.function((uint8_t)function);

        if(identity_number != -1)
            n.identity_number(identity_number);

        n.vehicle_system_instance(vehicle_system_instance);
        n.function_instance(function_instance);
        n.ecu_instance(ecu_instance);
    }
};

}}}