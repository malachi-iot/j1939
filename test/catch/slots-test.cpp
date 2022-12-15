#include <catch.hpp>

#include <j1939/slots.hpp>

#include "test-data.h"

using namespace embr::j1939;

TEST_CASE("slots")
{
    SECTION("SAEec01")
    {
        slot_type<slots::SAEec01> amps1{0}, amps2{800};
        embr::units::amps<int> a{0};

        a = amps1;

        REQUIRE(a.count() == -1600);
    }
    SECTION("SAEec02")
    {
        slot_type<slots::SAEec02> amps1{0}, amps2{125};
        embr::units::amps<int> a{0};

        a = amps1;

        REQUIRE(a.count() == -125);
    }
    SECTION("SAEtm12")
    {
        slot_type<slots::SAEtm12> time1{0};
        embr::units::hours<int> h{0};

        h = time1;

        REQUIRE(h.count() == -32127);
    }
    SECTION("SAEtm13")
    {
        typedef slot_traits<slots::SAEtm13> traits;
        traits::type time1{traits::max()};
        embr::units::microseconds<int> h{0};

        h = time1;

        REQUIRE(h.count() == 3289856);
    }
    SECTION("SAEtm14")
    {
        typedef slot_traits<slots::SAEtm14> traits;
        traits::type time1{traits::max()};
        embr::units::hours<int> h{0};

        h = time1;

        REQUIRE(h.count() == 6425);
    }
    SECTION("SAEtm14")
    {
        typedef slot_traits<slots::SAEtp01> traits;
        traits::type temp1{traits::max()};
        embr::units::centigrade<int> c{0};

        c = temp1;

        REQUIRE(c.count() == 210);
    }
    SECTION("SAEev01")
    {
        typedef slot_traits<slots::SAEev01> traits;
        traits::type v1{traits::max()};
        embr::units::millivolts<int> v{0};

        v = v1;

        REQUIRE(v.count() == 3212750);
    }
    SECTION("SAEtm10")
    {
        typedef slot_traits<slots::SAEtm10> traits;
        traits::type v1{0};
        embr::units::hours<int> v{v1};

        REQUIRE(v.count() == -125);

        v1 = 0x66;
        v = v1;

        REQUIRE(v.count() == -23);

        v1 = 0x94;
        v = v1;

        REQUIRE(v.count() == 23);
    }
    SECTION("SAEtm11")
    {
        typedef slot_traits<slots::SAEtm11> traits;
        traits::type v1{traits::max()};
        embr::units::hours<int> v{0};

        v = v1;

        REQUIRE(v.count() == 250);
    }
}