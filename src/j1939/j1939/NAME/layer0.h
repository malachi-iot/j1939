#pragma once

#include <embr/word.h>

#include "fwd.h"
#include "function.h"
#include "manufacturer.h"
#include "industry_groups.h"
#include "vehicle_systems.h"

namespace embr { namespace j1939 { namespace layer0 {

template <bool arbitrary_address_capable_, industry_groups ig_,
    vehicle_systems vs_,
    function_fields f_,
    // DEBT: Swap around mc and identity number, since identity number is 100% dependent
    // on mc, and not specifically the other way around
    int32_t identity_number_ = -1,
    manufacturer_codes mc_ = manufacturer_codes::reserved>
struct NAME
{
    static constexpr bool arbitrary_address_capable = arbitrary_address_capable_;
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
        n.industry_group((uint16_t)industry_group);
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

    template <bool aac>
    struct sparse_base : NAME<aac, ig_, vs_, f_, identity_number_, mc_>
    {
        uint16_t vehicle_system_instance_ : 4;
        uint16_t function_instance_ : 5;
        uint16_t ecu_instance_ : 3;

        // DEBT: Pretty sure I'd prefer a undefined/uninitialized
        // default constructor
        constexpr sparse_base() :
            vehicle_system_instance_{0},
            function_instance_{0},
            ecu_instance_{0}
        {}

        word<4> vehicle_system_instance() const
        {
            return word<4>(vehicle_system_instance_);
        }

        template <class TContainer>
        void populate(j1939::NAME<TContainer>& n)
        {
            NAME::populate(n,
                vehicle_system_instance(),
                function_instance_,
                ecu_instance_);
        }
    };

    // NOTE: Pulling this stunt so that we don't DIRECTLY derive from NAME
    // which some compilers complain isn't yet fully defined (true statement)
    using sparse = sparse_base<arbitrary_address_capable_>;
};

}}}
