#include <embr/scheduler.hpp>

#include <can/loopback.h>

#include <j1939/ca.hpp>

#include <j1939/cas/network.hpp>

#include "test-data.h"

using namespace estd::chrono_literals;
using namespace embr;
using namespace embr::j1939;

// This particular address manager stays in the strict arbitrary assigned
// range
struct SyntheticAddressManager
{
    unsigned seed = 0;

    void reset()
    {
        seed = 0;
    }

    void rotate()
    {
        seed <<= 3;
        seed ^= 123456789;
    }

    uint8_t peek_candidate()
    {
        unsigned v = seed % 120;
        return (uint8_t)v + 128;
    }

    uint8_t get_candidate()
    {
        const uint8_t v = peek_candidate();
        rotate();
        return v;
    }

    SyntheticAddressManager()
    {
        rotate();
    }
};

TEST_CASE("Controller Applications (network)")
{
    using namespace j1939;
    using frame = can::loopback_transport::frame;
    using frame_type = frame;
    using frame_traits = j1939::frame_traits<frame>;
    using address_traits = spn::internal::address_type_traits_base;

    can::loopback_transport t;

    embr::internal::layer1::Scheduler<5, FunctorImpl> scheduler;

    SECTION("address manager impl")
    {
        SECTION("synthetic")
        {
            SyntheticAddressManager am;
            unsigned v;

            v = am.get_candidate();
            REQUIRE(v == 197);
            v = am.get_candidate();
            REQUIRE(v == 181);
            v = am.get_candidate();
            REQUIRE(v == 133);
            v = am.get_candidate();
            REQUIRE(v == 221);
        }
    }
    SECTION("network")
    {
        impl::network_ca<decltype(t), decltype(scheduler), SyntheticAddressManager> impl(scheduler);
        const uint8_t addr = impl.address_manager.peek_candidate();

        // FIX: Can't quite find the get_helper it needs
        //test::setup_agricultural_planter(impl.name, 1, 0, 0);

        // request for address claimed
        pdu<pgns::request> r;
        // announce address claimed
        pdu<pgns::address_claimed> p_claim;

        r.payload().pgn((uint32_t)pgns::address_claimed);
        r.destination_address(address_traits::global);

        // BAM message, synthetic external party announcing claim of address
        p_claim.source_address(123);
        p_claim.destination_address(address_traits::global);
        // Setup all the nifty NAME parameters for external synthetic CA
        test::setup_trailer_brake(p_claim.payload());

        // Ensure loopback is in a good state before we begin
        REQUIRE(t.queue.empty());

        SECTION("CA ignores non-contentious claim")
        {
            // This CA will have a look at that request.  Address Claimed only
            // gets responded to if there's a collision, which in this case there
            // isn't
            process_incoming(impl, t, frame_traits::create(p_claim));

            REQUIRE(t.queue.empty());
        }
        SECTION("CA announces itself")
        {
            // Initiate network CA, creating announce
            impl.start(t);

            REQUIRE(!t.queue.empty());

            frame_traits::frame_type frame;

            bool received = t.receive(&frame);

            REQUIRE(received);

            // Brute force expected address to compare against
            // DEBT: this much access to given_address a no no
            p_claim.source_address(*impl.address());

            // Because it's a bit of work to convert a native CAN
            // to a PDU, we go the other direction for now
            auto compare_to_frame = frame_traits::create(p_claim);

            REQUIRE(compare_to_frame.id == frame.id);
            REQUIRE(compare_to_frame.dlc == frame.dlc);

            // DEBT: NAME won't match yet
        }
        SECTION("CA contends claim")
        {
            // Initiate network CA, creating announce
            impl.start(t);

            impl.address_manager.reset();

            p_claim.source_address(*impl.address());

            process_incoming(impl, t, frame_traits::create(p_claim));

            // Contains initial claim followed by condender claim
            REQUIRE(t.queue.size() == 2);

            frame_type f;

            t.receive(&f);
        }
        SECTION("CA has no internal SA, requests address claimed")
        {
            // FIX: This entire test is suspect, since it's indeterminate
            // if on start a CA should issue a request for address claimed

            // DEBT
            //impl.address.reset();

            impl.start(t);

            REQUIRE(!t.queue.empty());

            frame_type f;

            t.receive(&f);

            process_incoming(impl, t, f);
        }
        SECTION("randomness")
        {
            std::srand(std::time(nullptr));

            for(int i = 100; --i > 0;)
            {
                int v = impl.generate_preferred_sa();

                REQUIRE(v >= 128);
                REQUIRE(v < 254);
            }
        }
    }
}
