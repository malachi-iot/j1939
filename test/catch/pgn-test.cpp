#include <catch.hpp>

#include <j1939/data_field/all.hpp>


#include <j1939/pgn.h>
#include <j1939/spn.h>
#include <j1939/dispatcher.hpp>

#include <j1939/spn/descriptors.h>

#include "test-data.h"

using namespace embr::j1939;

TEST_CASE("pgn")
{
    SECTION("data_field")
    {
        SECTION("oel")
        {
            data_field<pgns::oel> data;
            data.turn_signal_switch();
            data.high_low_beam_switch(enum_type<spns::high_low_beam_switch>::high_beam_selected);
            //data.();
        }
        SECTION("lighting_command")
        {
            data_field<pgns::lighting_command> data;

            uint16_t val = data.get_raw<spns::left_turn_signal_lights_cmd>();

            REQUIRE(val == 0b11);

            data.set<spns::left_turn_signal_lights_cmd>(0);
            val = data.get_raw<spns::left_turn_signal_lights_cmd>();

            REQUIRE(val == 0);
        }
        SECTION("vehicle_electrical_power_1")
        {
            data_field<pgns::vehicle_electrical_power_1> data;
            typedef spn::traits<spns::battery_potential> traits_type;
            constexpr embr::units::millivolts<int> v1{25000};
            constexpr unit_type<spns::battery_potential> v2{v1};

            data.battery_potential(500);
            auto v = data.battery_potential();

            REQUIRE(v.count() == 500);

            data.battery_potential(v1);

            REQUIRE(v.count() == 500);
            REQUIRE(v.count() == v2.count());

            embr::units::volts<int> _v{v};

            REQUIRE(_v.count() == 25);
        }
        SECTION("transport_protocol")
        {
            data_field<pgns::tp_cm> data;

            // FIX: Something is wrong with underlying constexpr's
            data.control(data.cts);

            auto c = data.control();

            auto ts = data.total_size();

            data.pgn(20);
            data.max_packets(5);

            uint32_t _pgn = data.pgn();

            // FIX: Failing likely because we aren't passing '24' to 'width'
            REQUIRE(_pgn == 20);

            REQUIRE(data.max_packets() == 5);
        }
        SECTION("NMEA 2000")
        {
            SECTION("switch_bank_status")
            {
                data_field<pgns::switch_bank_status> data;

                bool v = data[4];
            }
        }
        SECTION("time_date")
        {
            data_field<pgns::time_date> data;
            embr::units::days<int> d = 10;

            data.seconds(3);
            data.minutes(3);
            data.hours(11);

            // NOTE: For some reason, they elected to go to 1/4 resolution with days, so this represents
            // one day
            data.day(4);

            REQUIRE(data.seconds().count() == 3);
            REQUIRE(data.day().count() == 4);

            embr::units::hours<int> h(data.day());

            REQUIRE(h.count() == 24);

            data.day(d);

            h = data.day();

            // 10 days = 240 hours
            REQUIRE(h.count() == 240);

            data.month(5);

            using local_hour_offset_traits = spn::type_traits<spns::local_hour_offset>;

            data.local_hour_offset(local_hour_offset_traits::raw::min());

            auto _h = data.local_hour_offset();
            h = _h;

            REQUIRE(h.count() == local_hour_offset_traits::cooked::min());
        }
    }
    SECTION("layer2 data_field")
    {
        uint8_t buf[32];
        //layer2::data_field<pgns::lighting_command> data(buf);

    }
}
