#include <estd/sstream.h>

#include <embr/scheduler.hpp>

#include <can/loopback.h>

#include <j1939/ca.hpp>

#include <j1939/cas/lighting_command.hpp>
#include <j1939/cas/network.hpp>
#include <j1939/cas/transport_protocol.hpp>
#include <j1939/cas/diagnostic.hpp>
#include <j1939/cas/oel_source.h>

#include <j1939/cas/internal/prng_address_manager.h>

#include "test-data.h"

#include "macro/push.h"

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
    inline bool process_incoming(transport_type&, pdu<pgn>) { return false; }

    int switch_bank_control_counter = 0;
    int oel_counter = 0;

    bool process_incoming(transport_type&, pdu<pgns::switch_bank_control>)
    {
        ++switch_bank_control_counter;
        return true;
    }

    bool process_incoming(transport_type& t, pdu<pgns::oel> p)
    {
        switch(p.turn_signal_switch())
        {
            case enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing:
            {
                ++oel_counter;
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

template <class TTransport>
struct SyntheticCA2 : j1939::impl::controller_application<TTransport>
{
    int unhandled_counter = 0;

    // Was experiencing inexplicable SIGTRAP here.  Turns out I forgot to
    // return a value and once again I was only warned (not error'd) about it
    bool process_incoming_default(TTransport&,
        const typename TTransport::frame&)
    {
        ++unhandled_counter;
        return false;
    }
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


using ostringstream = estd::detail::basic_ostream<estd::layer1::stringbuf<128>>;


TEST_CASE("Controller Applications")
{
    using namespace j1939;
    using frame = can::loopback_transport::frame;
    using frame_type = frame;
    using frame_traits = j1939::frame_traits<frame>;

    ostringstream out;
    const auto& out_s = out.rdbuf()->str();
    uint32_t can_id;

    can::loopback_transport t;

    const pdu<pgns::fms_identity> fmsi;   // Specifically, not a dispatched flavor

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
    SECTION("transport protocol")
    {
        impl::transport_protocol_ca<decltype(t)> impl_;
        using controls = pdu<pgns::tp_cm>::mode;

        pdu<pgns::tp_cm> r;

        r.payload().control(controls::rts);
        r.payload().pgn((uint32_t)pgns::NAME_management_message);

        process_incoming(impl_, t, frame_traits::create(r));
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
    SECTION("diagnostic ca")
    {
        diagnostic_ca<can::loopback_transport, ostringstream> dca(out);

        pdu<pgns::oel> p;

        frame f = frame_traits::create(p);

        process_incoming(dca, t, f);

        //REQUIRE(out_s == "OEL SA:0 ff ff ff ff ff ff ff ff \n");
        REQUIRE(out_s == "OEL SA:0 high beam=no change\n");
    }
    SECTION("aggregated")
    {
        SECTION("basic")
        {
            impl::controller_application_aggregator<
                SyntheticCA<decltype(t)>,
                SyntheticCA2<decltype(t)> > ca;

            //REQUIRE(sizeof(ca) == 12);

            auto& child1 = estd::get<0>(ca.child_cas);
            auto& child2 = estd::get<1>(ca.child_cas);

            pdu<pgns::oel> oel1;

            oel1.payload().turn_signal_switch(enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing);

            process_incoming(ca, t, frame_traits::create(oel1));
            process_incoming(ca, t, frame_traits::create(fmsi));

            REQUIRE(child1.oel_counter == 1);
            REQUIRE(child2.unhandled_counter == 1);
        }
    }
    SECTION("experimental")
    {
        embr::internal::layer1::Scheduler<5, FunctorImpl> scheduler;
        using address_manager = j1939::internal::prng_address_manager;
        using nca_type = j1939::impl::network_ca<decltype(t), decltype(scheduler), address_manager>;
        using proto_name = j1939::layer0::NAME<true,
            industry_groups::process_control,
            vehicle_systems::ig5_not_available, // DEBT: Change to a better IG/Veh Sys,
            function_fields::ig5_not_available>;
        using nca_init_type = nca_type::init1<proto_name::sparse>;
        nca_init_type nca_init(proto_name::sparse(0, 0, 0), scheduler);
        auto nca_init2 = nca_type::get_init(proto_name::sparse(0, 0, 0), scheduler);
        embr::j1939::layer1::NAME name;

        nca_type nca(nca_init2);

        estd::get<0>(nca_init).populate(name);

        REQUIRE(nca.name() == name);

        // TODO: Won't work yet because tuple doesn't yet have converting constructor
        //impl::controller_application_aggregator<nca_type> app_ca(nca_init2);
    }
    SECTION("sources")
    {
        // CA sources here are platform independent, which means they are largely a helper wrapper
        // around CAN Transport itself since the IO of the platform does most of the work

        SECTION("oel")
        {
            embr::j1939::ca::source::oel<can::loopback_transport> ca;
        }
    }
}

#include "macro/pop.h"

