/**
 *
 * References:
 *
 * 1. https://embeddeduse.com/2020/01/17/introduction-to-the-sae-j1939-standard/
 * 2. https://chortle.ccsu.edu/assemblytutorial/Chapter-15/ass15_3.html
 * 3. embr/bits/README.md
 * 4. J1939-81 (MAY2003)
 */
#pragma once

#include <catch2/catch.hpp>

#include <estd/cstdint.h>

#include <j1939/pdu.h>
#include <j1939/NAME/name.h>
#include <j1939/NAME/manufacturer.h>

constexpr static const uint8_t blinker_on[] =
    {};

namespace test_data {

const static embr::j1939::pdu2_header oel_id(0, embr::j1939::pgns::oel);

}

namespace test {

template <bool arbitrary_address,
    embr::j1939::manufacturer_codes mc = embr::j1939::manufacturer_codes::not_applicable,
    int32_t id = -1>
using NAME_trailer_brake = embr::j1939::layer0::NAME<
    arbitrary_address,
    embr::j1939::industry_groups::on_highway,
    embr::j1939::vehicle_systems::trailer,
    embr::j1939::function_fields::brakes_system_controller,
    id, mc>;

// setup NAME as per [4] C.1.2
template <class TContainer>
void setup_trailer_brake(embr::j1939::NAME<TContainer>& n)
{
    using namespace embr::j1939;

    n.arbitrary_address_capable(false);

    n.industry_group((uint8_t) industry_groups::on_highway);
    n.vehicle_system((uint8_t) vehicle_systems::trailer);

    n.vehicle_system_instance(1);
    n.function_instance(0);
    n.ecu_instance(0);

    n.function((uint8_t) function_fields::brakes_system_controller);

    n.manufacturer_code((uint16_t) manufacturer_codes::chrysler);
}

// setup NAME as per [4] C.1.3
template <class TContainer>
void setup_agricultural_planter(embr::j1939::NAME<TContainer>& n,
    embr::word<4> vehicle_instance, embr::word<3> ecu, embr::word<5> row)
{
    using namespace embr::j1939;

    n.arbitrary_address_capable(true);

    n.industry_group(2);

    embr::set(n, vehicle_systems::secondary_tillage);
    embr::set(n, function_fields::section_on_off_control);

    n.function_instance(row);
    n.vehicle_system_instance(vehicle_instance);
    n.ecu_instance(ecu);

    n.set_enum(manufacturer_codes::reserved);
    //n.identity_number()
}

}