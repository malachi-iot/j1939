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
    static constexpr int32_t identity_number = identity_number_;

    // NOTE: Does NOT match configurable fields such as instances
    template <class TContainer>
    constexpr static bool match(const j1939::NAME<TContainer>& n)
    {
        return
            n.arbitrary_address_capable() == arbitrary_address_capable &&
            vehicle_system == (vehicle_systems)n.vehicle_system() &&
            manufacturer_code == (manufacturer_codes)n.manufacturer_code() &&
            function == (function_fields)n.function();
    }
};

}}}