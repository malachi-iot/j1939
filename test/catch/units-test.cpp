#include <catch2/catch.hpp>

#include <estd/sstream.h>

#include <j1939/units/ostream.h>
#include <j1939/units/frequency.h>
#include <j1939/units/percent.h>
#include <j1939/units/operators.hpp>
#include <j1939/units/time.h>

using namespace embr::units;
using namespace estd::internal::units::literals;

using ostringstream = estd::detail::basic_ostream<estd::layer1::stringbuf<128>>;

TEST_CASE("units")
{
    ostringstream out;
    // FIX: Should work, but doesn't
    //estd::layer2::const_string out_s =
    const auto& out_s =
        out.rdbuf()->str();

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

        SECTION("ostream percent2")
        {
            write_abbrev(out, percent2);

            REQUIRE(out.rdbuf()->str() == "50%");
        }
        SECTION("ostream percent3")
        {
            // NOTE: Can't do this because rdbuf()->str() results in a const
            //out.rdbuf()->str().clear();

            write(out, percent3);

            REQUIRE(out_s == "50.70 percent");
        }
        SECTION("conversion")
        {
            // Need 100:1024 because ->
            // 512/1024 = 0.5 then we need * 100
            percent<int16_t, estd::ratio<100, 1024> > adc1{512};
            percent<int> p{adc1};
            percent<double> percent4{adc1};

            REQUIRE(p.count() == 50);

            percent3 = adc1;

            REQUIRE(percent3.count() == 50);

            adc1 = percent1;

            REQUIRE(adc1.count() == 519);
        }
    }
    SECTION("ostream")
    {
        //percent<double> p = 50_pct;   // FIX: It considers this narrowing, but wouldn't unsigned -> double be the opposite?
        percent<double> p = 50.0_pct;

        SECTION("regular")
        {
            out << put_unit(p);

            REQUIRE(out_s == "50.00%");
        }
        SECTION("non abbrev")
        {
            out << put_unit(p, false);

            REQUIRE(out_s == "50.00 percent");
        }
    }
    SECTION("misc")
    {
        // outlier tests that belong in embr/estd
        typedef estd::chrono::duration<uint8_t, estd::ratio<6, 10000>> type1;

        type1 d1(0), d2(255);

        REQUIRE(d1 == estd::chrono::milliseconds(0));
        REQUIRE(d2 == estd::chrono::milliseconds(153));
    }
    SECTION("operators")
    {
        // DEBT: 55_pct doesn't auto play nice with percent<double>

        percent<int16_t, estd::ratio<100, 1024> >
            adc_p1(512), adc_p2(100);
        percent<double> p1(55.0_pct);
        //percent<float> p2(45.0_pct);
        percent<float> p2(45.0);

        // would need conversion too, we're not there yet
        //auto v = p1 + p2;
        auto v = adc_p1 + adc_p2;

        REQUIRE(v.count() == 612);

        v = adc_p2 - adc_p1;

        REQUIRE(v.count() == -412);
    }
    // TODO: Move this out to embr
    SECTION("traits")
    {
        // Experimental and clumsy, but could be quite useful say to initialize
        // a bitrate-limited frequency with a percentage by way of a 'max' trait
        using traits = estd::internal::units::unit_traits<khz<int> >;
        auto min = traits::min();
        REQUIRE(min < 0);

        khz<int> dest = get_from_percent(53_pct, khz<int>(10000));

        REQUIRE(dest.count() == 5300);

        percent<int, estd::ratio<100, 1024>> p(50_pct);

        dest = get_from_percent(p, khz<int>(10000));

        REQUIRE(dest.count() == 5000);

        ++dest;

        REQUIRE(dest.count() == 5001);

        ++p;

        // FIX: Ambiguous operator
        REQUIRE(p == 50.09765625_pct);
    }
}
