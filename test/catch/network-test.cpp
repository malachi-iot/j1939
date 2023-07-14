#include <estd/chrono.h>

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

    static constexpr bool depleted() { return false; }

    void reset()
    {
        seed = 0;
    }

    // The most pseudo of psuedo random!
    void rotate()
    {
        seed <<= 3;
        seed ^= 123456789;
    }

    static void encountered(uint8_t) {}

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


struct PduDecomposer
{
    using frame = can::loopback_transport::frame;
    using frame_type = frame;
    using frame_traits = j1939::frame_traits<frame>;

    pdu1_header id;
    pdu2_header _id;

    pgns pgn = (pgns) (id.is_pdu1() ? id.range() : _id.range());

    estd::span<const uint8_t> payload;

    PduDecomposer(unsigned can_id) :
        id{can_id}, _id{can_id}
    {}

    PduDecomposer(unsigned can_id, const uint8_t* data, unsigned sz) :
        id{can_id}, _id{can_id},
        payload(data, sz)
    {}

    PduDecomposer(const frame& f) :
        id{f.id}, _id{f.id}, payload(f.payload, f.dlc) {}
};

constexpr unsigned saddresses[] = { 197, 181, 133, 221 };


TEST_CASE("Controller Applications (network)")
{
    using namespace j1939;
    using frame = can::loopback_transport::frame;
    using frame_type = frame;
    using frame_traits = j1939::frame_traits<frame>;
    using address_traits = spn::internal::address_type_traits_base;

    can::loopback_transport t;

    embr::internal::layer1::Scheduler<5, FunctorImpl> scheduler;
    j1939::layer1::NAME name;

    using network_ca = impl::network_ca<decltype(t), decltype(scheduler), SyntheticAddressManager>;

    // FIX: glitched out for the time being
    //test::setup_agricultural_planter(name, {0}, {0}, {0});

    test::NAME_trailer_brake<true>::populate(name);

    SECTION("address manager impl")
    {
        SECTION("synthetic")
        {
            SyntheticAddressManager am;
            unsigned v;

            // Semi arbitrary pseudo random test just for
            // sanity check
            v = am.get_candidate();
            REQUIRE(v == saddresses[0]);
            v = am.get_candidate();
            REQUIRE(v == saddresses[1]);
            v = am.get_candidate();
            REQUIRE(v == saddresses[2]);
            v = am.get_candidate();
            REQUIRE(v == saddresses[3]);
        }
    }
    SECTION("network")
    {
        network_ca impl(name, scheduler);
        const unsigned addr = impl.address_manager().peek_candidate();

        REQUIRE(addr == saddresses[0]);

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

            PduDecomposer pd{frame.id};

            REQUIRE(pd.pgn == pgns::address_claimed);
            REQUIRE(pd.id.source_address() == addr);

            // DEBT: NAME won't match yet
        }
        SECTION("CA contends claim (fully synthetic)")
        {
            // Initiate network CA, creating announce
            impl.start(t);

            impl.address_manager().reset();

            p_claim.source_address(*impl.address());

            process_incoming(impl, t, frame_traits::create(p_claim));

            // Contains initial claim followed by contender claim
            REQUIRE(t.queue.size() == 2);

            frame_type f;

            t.receive(&f);

            PduDecomposer pd{f};

            REQUIRE(pd.pgn == pgns::address_claimed);
            REQUIRE(pd.id.source_address() == addr);

            t.receive(&f);

            pd = f.id;

            REQUIRE(pd.pgn == pgns::address_claimed);
            // TODO: Not quite ready yet
            //REQUIRE((unsigned)pd.id.source_address() == addr);

            REQUIRE(!t.receive(&f));
        }
        SECTION("CA contends claim (2 SAs)")
        {
            j1939::layer1::NAME contender_name;
            test::names::agricultural_planter<true>::populate(contender_name);
            network_ca contender(contender_name, scheduler);

            REQUIRE(!(contender.name() == impl.name()));

            // Initiate primary CA, creating announce
            impl.start(t);

            // Initiate secondary (contender) CA, creating announce also on same address
            contender.start(t);

            // Contains initial claim followed by contender claim
            REQUIRE(t.queue.size() == 2);

            // Technically we should have something like two duplicated transports
            // to really simulate all this, but for now we'll fudge it
            frame_type f;

            // Yank out primary CA address claim
            REQUIRE(t.receive(&f));

            PduDecomposer pd{f};

            REQUIRE(pd.pgn == pgns::address_claimed);
            REQUIRE(pd.id.source_address() == addr);
            REQUIRE(f.dlc == 8);

            embr::j1939::layer1::NAME n(f.payload);

            REQUIRE(n == impl.name());

            // Evaluate contender first
            process_incoming(contender, t, f);

            REQUIRE(t.queue.size() == 2);

            // Yank out initial contender CA address claim
            t.receive(&f);

            pd = f;
            n = f.payload;

            REQUIRE(n == contender.name());
            REQUIRE(pd.pgn == pgns::address_claimed);
            REQUIRE(pd.id.source_address() == addr);
            REQUIRE(f.dlc == 8);

            // Yank out subsequence contender CA address claim with
            // new address in tow, since its NAME is lower priority than
            // primary CA
            t.receive(&f);

            pd = f;
            n = f.payload;

            REQUIRE(n == contender.name());
            REQUIRE(pd.pgn == pgns::address_claimed);
            REQUIRE((unsigned)pd.id.source_address() == saddresses[1]);

            REQUIRE(!t.receive(&f));

            process_incoming(impl, t, f);

            // TODO: Not quite sure whether we should be emitting something or not here
            // [AddressResolution.md] 1.1.3
            //REQUIRE(t.receive(&f));

            // DEBT: Why doesn't ms user literal work?
            scheduler.process(estd::chrono::milliseconds(11));

            REQUIRE(impl.state == network_ca::states::claiming);

            scheduler.process(estd::chrono::milliseconds(249));

            REQUIRE(impl.state == network_ca::states::claiming);

            scheduler.process(estd::chrono::milliseconds(250));

            REQUIRE(impl.state == network_ca::states::claimed);
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

            embr::j1939::impl::random_address_manager am;

            for(int i = 100; --i > 0;)
            {
                int v = am.get_candidate();

                REQUIRE(v >= 128);
                REQUIRE(v < 254);
            }
        }
    }
}
