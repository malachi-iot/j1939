#include <j1939/units/distance.h>
#include <j1939/units/time.h>

#include <j1939/state-machines/string_gatherer.hpp>

#include <embr/bits/bits.hpp>

#include <embr/services.hpp>

#include "test-data.h"

using namespace embr;

TEST_CASE("experimental")
{
    SECTION("A")
    {
        using unit = units::miles<int>::per<units::hours<int> >;

        unit mph{0};
    }
    SECTION("state machines")
    {
        j1939::data_field<j1939::pgns::tp_dt> v1, v2, v3;

        SECTION("string gatherer")
        {
            estd::copy_n("hi2u*", 5, v1.data());

            j1939::string_gatherer<estd::layer1::string<32> > sg;

            sg.process_incoming(v1);

            REQUIRE(sg.s.size() == 4);
            REQUIRE(sg.s == "hi2u");
        }
    }
    SECTION("services")
    {
        const int id = 0;
        services::manager.get(id);
    }
}