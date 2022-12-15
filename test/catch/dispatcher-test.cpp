#include <catch.hpp>

#include <map>


#include <embr/observer.h>

#include <j1939/dispatcher.hpp>

#include <j1939/pgn.h>

#include <can/loopback.h>

#include "test-data.h"

using namespace embr;
using namespace embr::j1939;

struct SyntheticObserver
{
    std::map<pgns, int> counters;

    void on_notify(events::received<pgns::oel> e)
    {
        ++counters[pgns::oel];
    }

    void on_notify(events::received<pgns::lighting_command> e)
    {
        ++counters[pgns::lighting_command];
    }
};


TEST_CASE("dispatcher")
{
    SECTION("dispatcher")
    {
        pdu2_header id{0};

        id.range((uint32_t)pgns::oel);

        auto s = embr::layer1::make_subject(SyntheticObserver());

        dispatch(id, blinker_on, s);

        auto& o = s.get<0>();

        int counter = o.counters[pgns::oel];

        REQUIRE(counter == 1);

        try
        {
            o.counters.at(pgns::lighting_command);
            FAIL("Should throw an exception");
        }
        catch(std::out_of_range)
        {

        }
    }
}