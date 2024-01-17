#include <catch2/catch.hpp>

#include <estd/sstream.h>

#include <j1939/pdu.h>

#include <j1939/data_field/cm1.hpp>
#include <j1939/data_field/cm3.hpp>
#include <j1939/data_field/dm01.hpp>
#include <j1939/data_field/fd.hpp>
#include <j1939/data_field/htr.hpp>
#include <j1939/data_field/oel.hpp>
#include <j1939/data_field/network.hpp>
#include <j1939/data_field/time.hpp>

using namespace embr::j1939;

#include <j1939/pgn/ostream.h>
#include <j1939/NAME/ostream.hpp>

#include "test-data.h"

using ostringstream = estd::detail::basic_ostream<estd::layer1::stringbuf<128>>;

#include "macro/push.h"

TEST_CASE("pdu")
{
    ostringstream out;
    const auto& out_s = out.rdbuf()->str();
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
    SECTION("cm1")
    {
        pdu<pgns::cab_message1> p;
        embr::units::percent<int> percent{50};

        p.requested_percent_fan_speed(percent);

        auto v = p.requested_percent_fan_speed();

        REQUIRE(v.count() == 125);

        out << embr::put_pdu(p);

        REQUIRE(out_s.starts_with("CM1 SA:0 DA:0 temp="));
    }
    SECTION("fd")
    {
        pdu<pgns::fan_drive_1> p;
        embr::units::percent<int> percent{50};

        p.estimated_percent_fan_speed(percent);

        auto v = p.estimated_percent_fan_speed();

        REQUIRE(v.count() == 125);

        p.fan_drive_state(0x7);

        REQUIRE(p.fan_drive_state() ==
            enum_type<spns::fan_drive_state>::default_operation);

        unsigned v2 = p.data_[1];

        REQUIRE(v2 == 0b11110111);
    }
    SECTION("dm01")
    {
        pdu<pgns::active_diagnostic_trouble_codes> p;
    }
    SECTION("oel")
    {
        pdu<pgns::oel> p;

        // TODO: Consider a mechanism which deduces which setter to use based solely on the unique
        // enum.  Would have limited use, but still useful in general
        p.turn_signal_switch(enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing);

        p.operators_desired_delay_lamp_off_time(10);
        auto v = p.delay_off_time();

        REQUIRE(v == estd::chrono::seconds(10));

        p.delay_off_time(estd::chrono::milliseconds(20000));

        REQUIRE(p.operators_desired_delay_lamp_off_time() == 20);
    }
    SECTION("time_date")
    {
        pdu<pgns::time_date> p;

        p.year(5);

        REQUIRE(p.year().count() == 1990);

        //embr::units::years<uint16_t> y(1996);

        //p.year(y);
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
    SECTION("address_claimed")
    {
        pdu<pgns::address_claimed> p;

        test::NAME_trailer_brake<true>::populate(p.payload());

        out << embr::put_pdu(p);

        REQUIRE(out_s == "AC SA:0 DA:0 ig1 vs=2:0 f=9:0 ecu=0 mc=7ff id=1fffff");
    }
}

#include "macro/pop.h"
