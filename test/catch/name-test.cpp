/**
 * References:
 *
 * 1. J1939-81 (MAY2003)
 * 2. J1939 (OCT2007)
 */
//#include <j1939/bits.hpp>

#include <j1939/NAME/name.h>
#include <j1939/NAME/manufacturer.h>

#include "test-data.h"

using namespace embr;

namespace embr { namespace j1939 { namespace experimental {

EMBR_J1939_GET_SET_ENUM(manufacturer_code, manufacturer_codes)

}}}

TEST_CASE("j1939-81 NAME")
{
    using NAME = j1939::NAME<estd::array<uint8_t, 8> >;

    constexpr uint32_t identity_number = 0x12345; // synthetic ID number for our own testing only

    // DEBT: Since NAME comes from data_field, it auto initializes to 0xFF.
    // We may want to deviate from that default for NAME specifically
    NAME n;

    n.identity_number(identity_number);

    SECTION("C.1 NAME examples")    // as per [1] C.1
    {
        SECTION("C.1.1 On-highway heavy duty truck engine ECU")
        {
            n.arbitrary_address_capable(false);

            n.industry_group((uint8_t) j1939::industry_groups::on_highway);
            n.vehicle_system((uint8_t) j1939::vehicle_systems::tractor);

            n.vehicle_system_instance(0);
            n.ecu_instance(0);
            n.function_instance(0);
            n.manufacturer_code((uint16_t) j1939::manufacturer_codes::allied_signal);

            n.function((uint8_t) j1939::function_fields::engine);

            auto ig = n.industry_group().cvalue();

            REQUIRE(ig == 1);

            word<21> v = n.identity_number();
            REQUIRE(n.identity_number() == identity_number);
        }
        SECTION("C.1.2 Brakes on the second trailer of heavy-duty truck")
        {
            test::setup_trailer_brake(n);

            REQUIRE(n.industry_group() == 1);
            REQUIRE(n.identity_number() == identity_number);
            REQUIRE(n.function() == (uint8_t)j1939::function_fields::brakes_system_controller);

            auto manufacturer_code = n.manufacturer_code();

            REQUIRE(manufacturer_code.cvalue() == (uint16_t) j1939::manufacturer_codes::chrysler);

            REQUIRE(n.get_enum<j1939::function_fields>() == j1939::function_fields::brakes_system_controller);
            REQUIRE(n.get_enum<j1939::manufacturer_codes>() == j1939::manufacturer_codes::chrysler);

            // NOTE: Does not compare instances, and in this case does not compare identity number
            // or manufacturer either
            bool v = test::NAME_trailer_brake<false>::match(n);

            REQUIRE(v);

            NAME n2;

            test::NAME_trailer_brake<false,
                j1939::manufacturer_codes::chrysler,
                identity_number>::populate(n2, {1});

            REQUIRE(n == n2);
        }
        SECTION("C.1.3 Agricultural Planter")
        {
            test::setup_agricultural_planter(n, 0, 0, 0);
            test::setup_agricultural_planter(n, 0, 0, 1);
            test::setup_agricultural_planter(n, 0, 0, 2);
            test::setup_agricultural_planter(n, 1, 0, 0);
        }
    }
    SECTION("auxiliary")
    {
        n.reserved(0);

        REQUIRE(n.reserved() == 0);

        n.reserved(1);

        REQUIRE(n.reserved() == 1);
    }
    SECTION("experimental")
    {
        embr::set(n, j1939::function_fields::brakes_system_controller);
        embr::set(n, j1939::industry_groups::on_highway);
        embr::set(n, j1939::vehicle_systems::trailer);

        REQUIRE(embr::get<j1939::function_fields>(n) == j1939::function_fields::brakes_system_controller);
        REQUIRE(embr::get<j1939::industry_groups>(n) == j1939::industry_groups::on_highway);
        REQUIRE(embr::get<j1939::vehicle_systems>(n) == j1939::vehicle_systems::trailer);

        n.set_enum(j1939::manufacturer_codes::qualcomm);

        REQUIRE(n.get_enum<j1939::manufacturer_codes>() == j1939::manufacturer_codes::qualcomm);
    }
    SECTION("layer0")
    {
        test::names::trailer_brake<true>::sparse name1;
        j1939::layer1::NAME name2;

        name1.populate(name2);

        REQUIRE(name2.arbitrary_address_capable());
        //REQUIRE(embr::get<j1939::industry_groups>(name2) ==
        REQUIRE((j1939::industry_groups)name2.industry_group().cvalue() ==
                j1939::industry_groups::on_highway);

        // Correctly is ambiguous, and qualifying by namespace
        // fixes that
        //test::names::synthetic_collider<true>::sparse name3;
        test::names::ig2::synthetic_collider<true>::sparse name3;
    }
}
