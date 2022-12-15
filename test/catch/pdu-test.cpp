#include <catch.hpp>

#include <j1939/pdu.h>

#include <j1939/data_field/oel.hpp>
#include <j1939/data_field/network.hpp>

using namespace embr::j1939;

TEST_CASE("pdu")
{
    uint32_t can_id;

    SECTION("headers")
    {
        SECTION("pdu2")
        {
            pdu2_header id(5, pgns::oel);

            REQUIRE(id.range() == (uint32_t)pgns::oel);
            REQUIRE(id.priority() == 5);
        }
    }
    SECTION("A")
    {
        pdu<pgns::oel> p;

        // TODO: Consider a mechanism which deduces which setter to use based solely on the unique
        // enum.  Would have limited use, but still useful in general
        p.turn_signal_switch(enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing);
    }
    SECTION("time_date")
    {
        pdu<pgns::time_date> p;
    }
    SECTION("commanded_address")
    {
        // Also includes implicit NAME testing
        pdu<pgns::commanded_address> p;

        p.can_id().source_address(77);
        p.payload().source_address(7);

        REQUIRE(p.can_id().source_address() == 77);
        REQUIRE(p.payload().source_address() == 7);

        p.payload().identity_number(0x1234);
    }
}