#include <embr/bits/bits.hpp>

#include "test-data.h"

using namespace embr;

TEST_CASE("bits2")
{
    uint8_t raw[64];
    std::fill_n(raw, sizeof(raw), 0);

    SECTION("byte operations")
    {
        SECTION("can_id::priority")
        {
            j1939::pdu2_header id(5, j1939::pgns::oel);

            auto priority = id.priority();

            REQUIRE(priority == 5);
            REQUIRE(estd::numeric_limits<decltype(priority)>::max() == 7);
        }
    }
}