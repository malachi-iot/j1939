#include <catch2/catch.hpp>


#include <j1939/units/amps.h>
#include <j1939/units/celsius.h>
#include <j1939/units/distance.h>
#include <j1939/units/frequency.h>
#include <j1939/units/fahrenheit.h>
#include <j1939/units/kelvins.h>
#include <j1939/units/percent.h>
#include <j1939/units/rpm.h>
#include <j1939/units/si.h>
#include <j1939/units/volts.h>
#include <j1939/units/watts.h>

#include <j1939/units/operators.hpp>

using namespace embr::units;

TEST_CASE("units")
{
    SECTION("centigrade")
    {
        // lifted right from SPN 4420
        centigrade<uint8_t, estd::ratio<1>, embr::units::internal::adder<int16_t, -40> > c1{0};
        centigrade<int> c2{c1};

        REQUIRE(c1.count() == -40);
        REQUIRE(c2.count() == -40);
    }
    SECTION("amps")
    {
        milliamps<uint16_t> ma{3600};
        amps<uint8_t> a = ma;

        REQUIRE(a.count() == 3);
    }
    SECTION("volts")
    {
        millivolts<uint16_t> mv{3600};
        decivolts<uint8_t> dv{mv};
        volts<uint8_t> v{mv};

        REQUIRE(dv.count() == 36);
        REQUIRE(v.count() == 3);
    }
    SECTION("rpm")
    {
        rpm<uint16_t> rpm1{5000};
        rpm<uint16_t, estd::ratio<1, 2> > rpm2{rpm1};

        REQUIRE(rpm2.count() == 10000);
    }
    SECTION("percent")
    {
        percent<uint16_t, estd::ratio<1, 10> > percent1{974};

        REQUIRE(percent<uint8_t>(percent1).count() == 97);

        auto percent2 = 50_pct;

        REQUIRE(percent2.count() == 50);

        percent<double> percent3{50.1};

        percent1 = percent3;

        REQUIRE(percent1.count() == 501);

        percent1.root_count(507);
        percent3 = percent1;

        REQUIRE(percent3.count() == 50.7);

        REQUIRE(percent3 == 50.7_pct);
        REQUIRE(percent1 == percent3);

        // lhs precision is lower so dangling .7% falls away
        REQUIRE(50_pct == percent1);
    }
    SECTION("conversions")
    {
        volts<uint8_t> v{120};
        watts<uint16_t> w{1000};

        // EXPERIMENTAL, not ready
        //milliamps<uint16_t> ma = w / v;

        // FIX: We want to prohibit this
        //v = a;
    }
    SECTION("rates/speed")
    {
        SECTION("meters per second")
        {
            constexpr kilometers_per_hour<uint8_t> kph(60);
            meters_per_second<uint16_t> m_s{kph};

            REQUIRE(m_s.count() == 60000 / 3600);
        }
    }
}
