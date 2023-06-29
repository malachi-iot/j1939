/**
 *
 * References:
 *
 * 1. J1939-81 (draft MAY2003)
 * 2. J1939-21 (DEC2006)
 */
#pragma once
#include <bitset>       // DEBT, make an estd one and use it

#include <estd/functional.h>
#include <estd/optional.h>

#include "../ca.h"
#include "../data_field/network.hpp"
#include "../data_field/transport_protocol.hpp"

#include "fwd.h"

namespace embr { namespace j1939 {

namespace impl {

// this one will use a lot of RAM
template <size_t max>
struct network_address_and_name_tracker
{
    pdu<pgns::address_claimed> claims[max];
};


template <size_t max>
struct network_address_tracker
{
    // starting from 128
    std::bitset<max> claimed;
};

namespace experimental {

// FIX: This is all sloppy, just doing this to bring things online.  We may instead consider always
// demanding chrono for CA scheduler impl time_point

template <class TSchedulerImpl, class TTimePoint = typename TSchedulerImpl::time_point>
struct ca_time_helper;

template <class TSchedulerImpl>
struct ca_time_helper<TSchedulerImpl, unsigned>
{
    static constexpr unsigned milliseconds(unsigned ms) { return ms; }
};

template <class TSchedulerImpl, class Rep, class Period>
struct ca_time_helper<TSchedulerImpl, estd::chrono::duration<Rep, Period> >
{
    typedef estd::chrono::duration<Rep, Period> time_point;

    static constexpr time_point milliseconds(Rep ms)
    {
        return estd::chrono::milliseconds(ms);
    }
};

}

struct ca_base
{
    template <pgns pgn>
    using pdu = pdu<pgn>;
};

struct network_ca_base : ca_base
{
    // DEBT: Merge this with 'service' architecture
    enum class states
    {
        unstarted,
        claiming,
        claimed,
        claim_failed
    };


    // Guidance from [1] Appendix D: State Transition Diagrams
    enum class substates
    {
        unstarted,

        // claiming state
        waiting,            ///< Waiting period after we emit a claim address
        contending,         ///< Evaluation period after we receive a contending address
        bus_off,
        bus_off_recover,

        // claimed state
        expired,            ///< Claim waiting period expired with no incident, meaning we succeeded
    };

    states state = states::unstarted;
    substates substate = substates::unstarted;

    layer1::NAME name;

    inline bool arbitrary_address_capable() const
    {
        return name.arbitrary_address_capable();
    }
};

// Pertains to [1] 5.10
template <class TTransport, class TScheduler>
struct network_ca : impl::controller_application<TTransport>,
                    network_ca_base
{
    typedef j1939::impl::controller_application<TTransport> base_type;
    using typename base_type::transport_type;
    using typename base_type::frame_type;
    using typename base_type::frame_traits;

    typedef transport_traits<transport_type> _transport_traits;

    // TODO: Change to non-optional (since state machine handles that)
    // TODO: Change name to 'address' as it may be claimed, claiming or cannot claim depending on
    // state machine
    estd::layer1::optional<uint8_t, spn::internal::address_type_traits_base::null> given_address;

    typedef TScheduler scheduler_type;

    scheduler_type& scheduler;

    // DEBT: Instead, expose impl_type directly from sechduler_type
    typedef estd::remove_reference_t<decltype(scheduler.impl())> scheduler_impl_type;
    typedef typename scheduler_impl_type::time_point time_point;
    typedef estd::experimental::function<void(time_point*, time_point)> function_type;
    //typedef impl::experimental::ca_time_helper<scheduler_impl_type> helper;

    // DEBT: Not all time_point will be chrono-compatible
    //static constexpr time_point timeout() { return helper::milliseconds(250); }
    static constexpr time_point timeout() { return estd::chrono::milliseconds(250); }

    function_type f;

    time_point last_claim;
    transport_type* t;

    void send_cannot_claim(transport_type& t, pdu<pgns::address_claimed>& p)
    {
        p.can_id().source_address(spn::internal::address_type_traits_base::null);
        _transport_traits::send(t, p);

    }

    void send_cannot_claim(transport_type& t)
    {
        pdu<pgns::address_claimed> p;

        p.can_id().destination_address(spn::internal::address_type_traits_base::global);
        p.payload() = name;

        send_cannot_claim(t, p);
    }

    void send_claim(transport_type& t, pdu<pgns::address_claimed>& p)
    {
        p.can_id().source_address(*given_address);
        _transport_traits::send(t, p);

        last_claim = scheduler.impl().now();
    }

    void send_claim(transport_type& t)
    {
        pdu<pgns::address_claimed> p;

        p.payload() = name;
        p.can_id().destination_address(spn::internal::address_type_traits_base::global);

        send_claim(t, p);
    }

    // [1] Figure D1
    inline bool skip_timeout()
    {
        return (given_address >= 0 && given_address <= 127) ||
               (given_address >= 248 && given_address <= 253);
    }

    inline bool schedule_timeout(time_point* wake)
    {
        if(skip_timeout()) return false;

        *wake += timeout();
        return true;
    }


    void scheduled_claiming(time_point* wake, time_point current);

    void scheduled_cannot_claim(time_point* wake, time_point current)
    {
        switch(substate)
        {
            //case substates::
            default:
                break;
        }
    }

    void scheduled(time_point* wake, time_point current)
    {
        switch(state)
        {
            case states::claiming:
                scheduled_claiming(wake, current);
                break;

            case states::claim_failed:
                scheduled_cannot_claim(wake, current);
                break;

            default:
                break;
        }
    }


    network_ca(scheduler_type& scheduler) : scheduler{scheduler},
                                            f([&](time_point* wake, time_point current) { scheduled(wake, current); })
    {

    }

    // DEBT: I think we'd prefer to do this at constructor, but for now is easier
    // to do a manual start call
    void start(transport_type& t)
    {
        this->t = &t;

        send_claim(t);

        scheduler.schedule(last_claim + timeout(), f);
    }

    inline bool is_contender(const pdu<pgns::address_claimed>& p)
    {
        return p.source_address() == given_address;
    }

    void evaluate_contender(transport_type& t, const pdu<pgns::address_claimed>& p)
    {
        state = states::claiming;
        substate = substates::contending;
    }

    template <pgns pgn>
    inline bool process_incoming(transport_type& t, pdu<pgn> p) { return false; }

    bool process_incoming(transport_type& t, const pdu<pgns::address_claimed>& p);

    bool process_request_for_address_claimed(transport_type& t, const pdu<pgns::request>& p);

    bool process_incoming(transport_type& t, const pdu<pgns::request>& p);
};


}

}}