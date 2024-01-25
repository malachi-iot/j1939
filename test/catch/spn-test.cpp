#include <catch2/catch.hpp>

#include <j1939/spn.h>
#include <j1939/spn/traits.h>

#include <j1939/spn/descriptors.h>

#include <j1939/data_field/all.hpp>
#include <j1939/units/celsius.h>
#include <j1939/spn/units.h>


using namespace embr::j1939;
using namespace embr::units::literals;


TEST_CASE("spn")
{
    SECTION("ranges")
    {
        SECTION("Valid Signal")
        {
            REQUIRE(spn::range_traits<uint16_t>::valid_signal(64025) == true);
            REQUIRE(spn::range_traits<uint16_t>::valid_signal(64300) == false);
        }
        SECTION("Error Indicator")
        {
            REQUIRE(spn::range_traits<uint16_t>::error(0xFE05) == true);
            REQUIRE(spn::range_traits<uint16_t>::error(0xFD05) == false);
        }
        SECTION("Parameter Specific")
        {
            REQUIRE(spn::range_traits<uint16_t>::parameter_specific(0xFB23) == true);
            REQUIRE(spn::range_traits<uint16_t>::parameter_specific(0xFD05) == false);
        }
    }
    SECTION("traits")
    {
        typedef spn::traits<spns::left_stop_lights_cmd> traits_type;

        REQUIRE(traits_type::noop(3, false) == true);

        SECTION("runtime compare")
        {
            SECTION("lighting")
            {
                data_field<pgns::lighting_command> payload;
                //using ut = unit_type<spns::left_stop_lights_cmd>;

                payload.left_stop(traits_type::enum_type::noop);

                int v = (int)payload.left_stop();

                REQUIRE(traits_type::noop(v, false));
            }
            SECTION("cm1")
            {
                data_field<pgns::cab_message1> payload;
                using traits = spn::traits<spns::cab_interior_temperature_command>;
                //typedef slot_traits<slots::SAEtp02> traits;

                using st = slot_type<slots::SAEtp02>;
                //using ut = unit_type<spns::cab_interior_temperature_command>();

                st temp(payload.cab_interior_temperature_command());

                int v = temp.root_count();

                REQUIRE(v == 0xFFFF);

                bool v2 = traits::noop(temp);

                REQUIRE(v2);

                // although predictable, values are undefined if noop is seen
                //v = temp.count();

                //embr::units::celsius<short> v2(temp);

                //REQUIRE(v2.count() == 1337);
            }
        }
    }
    SECTION("placeholder")
    {
        //auto d = spn_traits<pgns::lighting_command, spns::left_turn_signal_lights_cmd>::descriptor();
        auto d = spn::get_descriptor<spns::left_turn_signal_lights_cmd>();
    }
    SECTION("units")
    {
        SECTION("slot")
        {
            slot::unit<slots::SAEtp02> v(0);

            v += 5_celsius;

            spn::unit<spns::cab_interior_temperature_command> v3(5_celsius);

            REQUIRE(v3 == v);
        }
        SECTION("status (control commands)")
        {
            spn::unit<spns::request_cab_zone_heating> v;

            v = spn::control_commands::enable;

            REQUIRE(v == spn::control_commands::enable);

            spn::control_commands v2 = v;
        }
        SECTION("measured (discrete parameters)")
        {
            spn::unit<spns::joystick1_button1_pressed_status> v;

            v = spn::measured::enabled;

            REQUIRE(v == spn::discrete_parameters::enabled);
        }
    }
}
