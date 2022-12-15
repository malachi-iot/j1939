#include <catch.hpp>

#include <j1939/spn.h>
#include <j1939/spn/traits.h>

#include <j1939/spn/descriptors.h>

#include <j1939/data_field/all.hpp>


using namespace embr::j1939;

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
    }
    SECTION("placeholder")
    {
        //auto d = spn_traits<pgns::lighting_command, spns::left_turn_signal_lights_cmd>::descriptor();
        auto d = spn::get_descriptor<spns::left_turn_signal_lights_cmd>();
    }
}
