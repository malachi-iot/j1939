#include <catch2/catch.hpp>

#include <can/loopback.h>
#include <can/aggregated_transport.h>

#include <j1939/ca.hpp>

using namespace embr::can;

#include "macro/push.h"

TEST_CASE("transport (can)")
{
    SECTION("loopback")
    {
        using frame_type = loopback_transport::frame;

        embr::j1939::pdu2_header _id1{0}, _id2{0};

        _id1.range((uint32_t )embr::j1939::pgns::oel);
        _id2.range((uint32_t )embr::j1939::pgns::lighting_command);

        estd::span<uint8_t> span((uint8_t*)"hi2u", 4);
        int counter = 0;
        auto f = estd::experimental::function<
            bool(const loopback_transport::frame&)>::make_inline2(
            [&](const loopback_transport::frame& message)
            {
                ++counter;
                return false;
            });

        loopback_transport t;

        t.receive_callback = f;
        t.send(0, span);

        REQUIRE(counter == 1);
    }
    SECTION("aggregated")
    {
        using frame_type = loopback_transport::frame;
        frame_type f;
        int idx = -1;

        SECTION("basic")
        {
            aggregated_transport<loopback_transport> at;

            at.send(f);
            bool result = at.receive(&f, &idx);

            REQUIRE(result);
            REQUIRE(idx == 0);

            result = at.receive(&f);

            REQUIRE(result == false);
        }
        SECTION("multiple")
        {
            // this one has 2 loopbacks
            loopback_aggregated_transport<loopback_transport> at;

            at.send(f);

            bool result = at.receive(&f, &idx);

            REQUIRE(result);
            REQUIRE(idx == 1);

            result = at.receive(&f, &idx);

            REQUIRE(result);
            REQUIRE(idx == 0);

            result = at.receive(&f, &idx);

            REQUIRE(result == false);
        }
    }
    SECTION("traits")
    {
        using namespace embr::j1939;

        using frame_type = loopback_transport::frame;
        using frame_traits = embr::j1939::frame_traits<frame_type>;
        using address_traits = spn::internal::address_type_traits_base;
    }
}

#include "macro/pop.h"

