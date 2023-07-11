#include <embr/scheduler.hpp>

#include <can/loopback.h>

#include <j1939/ca.hpp>

#include <j1939/cas/lighting_command.hpp>
#include <j1939/cas/network.hpp>
#include <j1939/cas/transport_protocol.hpp>

#include "test-data.h"

using namespace estd::chrono_literals;
using namespace embr;
using namespace embr::j1939;

template <class TTransport>
struct SyntheticCA : j1939::impl::controller_application<TTransport>
{
    typedef j1939::impl::controller_application<TTransport> base_type;
    using typename base_type::transport_type;
    using typename base_type::frame_type;
    using typename base_type::frame_traits;

    template <pgns pgn>
    using pdu = pdu<pgn>;

    typedef transport_traits<transport_type> _transport_traits;

    template <pgns pgn>
    inline bool process_incoming(transport_type& t, pdu<pgn> p) { return false; }

    int switch_bank_control_counter = 0;

    inline bool process_incoming(transport_type& t, pdu<pgns::switch_bank_control> p)
    {
        ++switch_bank_control_counter;
        return true;
    }

    inline bool process_incoming(transport_type& t, pdu<pgns::oel> p)
    {
        switch(p.turn_signal_switch())
        {
            case enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing:
            {
                // DEBT: Not really a great command/response chain, but better than a pure echoback
                pdu<pgns::switch_bank_control> pdu_response;

                _transport_traits::send(t, pdu_response);
                break;
            }

            default:
                return false;
        }

        return true;
    }
};


struct FunctorImpl : embr::internal::scheduler::impl::Function<estd::chrono::milliseconds>
{
    estd::chrono::milliseconds now_ = estd::chrono::milliseconds::zero();

    estd::chrono::milliseconds now() const { return now_; }
};

namespace embr { namespace  j1939 { namespace  impl { namespace  experimental {

template <>
struct ca_time_helper<FunctorImpl, unsigned>
{
    static constexpr unsigned milliseconds(unsigned ms) { return ms; }
};

}}}}


struct BlinkCAImpl
{
    // number of total blinks
    int left_count = 0;
    int right_count = 0;

    void left_signal(bool on)
    {
        ++left_count;
    }

    void right_signal(bool on)
    {
        ++right_count;
    }
};


TEST_CASE("Controller Applications")
{
    using namespace j1939;
    using frame = can::loopback_transport::frame;
    using frame_type = frame;
    using frame_traits = j1939::frame_traits<frame>;

    can::loopback_transport t;

    SECTION("basics")
    {
        controller_application<decltype(t), SyntheticCA<decltype(t)> > ca;

        pdu<pgns::oel> oel1;

        oel1.payload().turn_signal_switch(enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing);

        frame f = frame_traits::create(oel1);

        ca.process_incoming(t, f);

        const frame* f_received = t.receive();

        REQUIRE(f_received != nullptr);

        REQUIRE(ca.switch_bank_control_counter == 0);

        REQUIRE(ca.process_incoming(t, *f_received));

        REQUIRE(ca.switch_bank_control_counter == 1);
    }
    SECTION("aggregated")
    {
        impl::controller_application_aggregator<SyntheticCA<decltype(t)> > ca;

        pdu<pgns::oel> oel1;

        oel1.payload().turn_signal_switch(enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing);

        process_incoming(ca, t, frame_traits::create(oel1));
    }
    // DEBT: Almost certainly this particular CA deserves its own test module
    SECTION("network")
    {
        using address_traits = spn::internal::address_type_traits_base;

        embr::internal::layer1::Scheduler<5, FunctorImpl> scheduler;
        impl::network_ca<decltype(t), decltype(scheduler)> impl(scheduler);

        // DEBT
        impl.given_address = 77;

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
            p_claim.source_address(*impl.given_address);

            // Because it's a bit of work to convert a native CAN
            // to a PDU, we go the other direction for now
            auto compare_to_frame = frame_traits::create(p_claim);

            REQUIRE(compare_to_frame.id == frame.id);
            REQUIRE(compare_to_frame.dlc == frame.dlc);

            // DEBT: NAME won't match yet
        }
        SECTION("CA contends claim")
        {
            p_claim.source_address(*impl.given_address);

            process_incoming(impl, t, frame_traits::create(p_claim));

            REQUIRE(!t.queue.empty());

            frame_type f;

            t.receive(&f);
        }
        SECTION("CA has no internal SA, requests address claimed")
        {
            // DEBT
            impl.given_address.reset();

            impl.start(t);

            REQUIRE(!t.queue.empty());

            frame_type f;

            t.receive(&f);

            process_incoming(impl, t, f);
        }
    }
    SECTION("transport protocol")
    {
        impl::transport_protocol_ca<decltype(t)> impl;
        using controls = pdu<pgns::tp_cm>::mode;

        pdu<pgns::tp_cm> r;

        r.payload().control(controls::rts);
        r.payload().pgn((uint32_t)pgns::NAME_management_message);

        process_incoming(impl, t, frame_traits::create(r));
    }
    SECTION("lighting command (ca)")
    {
        embr::internal::layer1::Scheduler<5, FunctorImpl> scheduler;
        impl::lighting_command_ca<decltype(t), decltype(scheduler)> ca(scheduler);
        can::loopback_transport::frame frame;

        pdu<pgns::oel> p;

        p.turn_signal_switch(enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing);

        ca.process_incoming(t, p);

        REQUIRE(t.queue.size() == 0);   // no lighting commands are generated until scheduler runs

        scheduler.impl().now_ += estd::chrono::milliseconds(300);
        scheduler.process();
        REQUIRE(t.queue.size() == 1);
        REQUIRE(t.receive(&frame));
        REQUIRE(t.queue.size() == 0);
        scheduler.impl().now_ += estd::chrono::milliseconds(300);
        scheduler.process();
        REQUIRE(t.queue.size() == 1);
        REQUIRE(t.receive(&frame));
    }
}
