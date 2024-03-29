#include <estd/sstream.h>

#include <j1939/ostream.h>

#include "test-data.h"

#include <j1939/data_field/time.hpp>

using ostringstream = estd::detail::basic_ostream<estd::layer1::stringbuf<128>>;

using namespace embr;
using namespace embr::j1939;

TEST_CASE("ostream")
{
    ostringstream out;
    const auto& out_s = out.rdbuf()->str();

    SECTION("NAME")
    {
        test::names::trailer_brake<true>::sparse name1;
        j1939::layer1::NAME name2;

        name1.populate(name2);

        //out << embr::j1939::put_payload(name2);

        out << j1939::internal::NAME_payload_put{name2};

        REQUIRE(out_s == "ig1 vs=2:0 f=9:0 ecu=0 mc=2047 id=1fffff");
    }
    SECTION("pdu_header")
    {
        SECTION("pdu1")
        {
            pdu1_header ph{0};

            out << ph;

            REQUIRE(out_s == "SA:0 DA:0");
        }
        SECTION("pdu2")
        {
            pdu2_header ph{0};

            out << ph;

            REQUIRE(out_s == "SA:0");
        }
    }
    SECTION("data_field")
    {
        SECTION("oel")
        {
            data_field<pgns::oel> payload;

            out << estd::hex << j1939::internal::payload_put<pgns::oel>{payload};

            //REQUIRE(out_s == "ff ff ff ff ff ff ff ff ");
            REQUIRE(out_s == "high beam=no change");
        }
        SECTION("time_data")
        {
            data_field<pgns::time_date> payload;
            embr::units::years<uint16_t> year(1990);

            payload.year(5);    // DEBT: Year is 1985+ this value.  Make year truly take human readable value
            //payload.year(year);
            payload.month(12);
            payload.day(25);
            payload.hours(10);
            payload.minutes(30);
            payload.seconds(55);

            out << j1939::internal::payload_put<pgns::time_date>{payload};

            REQUIRE(out_s == "1990-12-25T10:30:55");
        }
    }
    SECTION("can_id")
    {
        pdu<pgns::oel> p;

        out << p.can_id();

        REQUIRE(out_s == "SA:0");
    }
    SECTION("pdu")
    {

    }
}