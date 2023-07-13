/**
 *
 * References:
 *
 * 1. J1939-81 (draft MAY2003)
 * 2. J1939-21 (DEC2006)
 */
#pragma once
#include <bitset>       // DEBT, make an estd one and use it

#include <estd/chrono.h>
#include <estd/functional.h>
#include <estd/optional.h>

#include <embr/service.h>

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

struct network_ca_base : ca_base,
    embr::Service   // Ready and waiting, premature to start migrating to this atm
{
    // DEBT: Upgrade this to embr 'service' architecture
    enum class states
    {
        unstarted,
        requesting,         ///< Request for address_claimed [2] A5, A6, A7 Initialize
        claiming,           ///< Address Claim - emit and wait
        claimed,            ///< Address Claim success without contention
        claim_failed
    };


    // Guidance from [1] Appendix D: State Transition Diagrams
    enum class substates
    {
        unstarted,

        // requesting state
        request_waiting,    ///< Waiting period (1250ms) after we emit a request for address claim

        // claiming state
        waiting,            ///< Waiting period (250ms) after we emit a claim address
        contending,         ///< Evaluation period after we receive a contending address
        bus_off,
        bus_off_recover,

        // claimed state
        expired,            ///< Claim waiting period expired with no incident, meaning we succeeded
    };

    states state = states::unstarted;
    substates substate = substates::unstarted;

    using address_traits = spn::internal::address_type_traits_base;
    using address_type = estd::layer1::optional<uint8_t, address_traits::null>;

protected:
    layer1::NAME name;

    // TODO: Change to non-optional (since state machine handles that)
    // May be claimed, claiming or cannot claim depending on
    // state machine
    address_type address_;

public:
    const address_type& address() const { return address_; }

    using milliseconds = estd::chrono::milliseconds;

    static constexpr milliseconds address_claim_timeout() { return milliseconds(250); }
    static constexpr milliseconds request_for_address_claim_timeout()
    {
        return milliseconds(1250);
    }

    constexpr bool arbitrary_address_capable() const
    {
        return name.arbitrary_address_capable();
    }
};

// Pertains to [1] 5.10
template <class TTransport, class TScheduler, class TAddressManager>
struct network_ca : impl::controller_application<TTransport>,
                    network_ca_base
{
    typedef j1939::impl::controller_application<TTransport> base_type;
    using typename base_type::transport_type;
    using typename base_type::frame_type;
    using typename base_type::frame_traits;

    typedef transport_traits<transport_type> _transport_traits;

    typedef TScheduler scheduler_type;
    typedef TAddressManager address_manager_type;

    scheduler_type& scheduler;
    address_manager_type address_manager;

    // DEBT: Instead, expose impl_type directly from sechduler_type
    typedef estd::remove_reference_t<decltype(scheduler.impl())> scheduler_impl_type;
    typedef typename scheduler_impl_type::time_point time_point;
    typedef typename scheduler_impl_type::function_type fn_test;
    // DEBT: Using dynamic allocated function, we strongly frown on this.  Being
    // that we do not expect to free it, this is not a FIX
    //typedef estd::experimental::function<void(time_point*, time_point)> old_function_type;

    using function_type = typename scheduler_impl_type::function_type;
    //typedef impl::experimental::ca_time_helper<scheduler_impl_type> helper;

    //function_type f;

    time_point last_claim;
    transport_type* t;

    uint8_t generate_preferred_sa();

    address_type find_new_address() const { return {}; }   // NOLINT

    constexpr bool has_address() const
    {
        return address_.has_value();
    }

    // [1] 4.2.1
    void send_request_for_address_claimed(
            transport_type&,
            uint8_t dest = address_traits::global);

    void send_cannot_claim(transport_type& t, pdu<pgns::address_claimed>& p)
    {
        p.can_id().source_address(address_traits::null);
        _transport_traits::send(t, p);

    }

    void send_cannot_claim(transport_type& t)
    {
        pdu<pgns::address_claimed> p;

        p.can_id().destination_address(address_traits::global);
        p.payload() = name;

        send_cannot_claim(t, p);
    }

    void send_claim(transport_type& t, pdu<pgns::address_claimed>& p, uint8_t sa)
    {
        // DEBT: Not sure if claim ALWAYS is a BAM but I think so
        p.can_id().destination_address(address_traits::global);
        p.payload() = name;
        p.can_id().source_address(sa);
        _transport_traits::send(t, p);

        last_claim = scheduler.impl().now();
    }

    void send_claim(transport_type& t)
    {
        pdu<pgns::address_claimed> p;

        send_claim(t, p, *address_);
    }

    // [1] Figure D1
    constexpr bool skip_timeout() const
    {
        return (address_ >= 0 && address_ <= 127) ||
               (address_ >= 248 && address_ <= 253);
    }

    bool schedule_address_claim_timeout(time_point* wake)
    {
        if(skip_timeout()) return false;

        *wake += address_claim_timeout();
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

    struct wake_functor
    {
        network_ca& this_;

        void operator()(time_point* wake, time_point current)
        {
            this_.scheduled(wake, current);
        }
    };

    typename function_type::template model<wake_functor> wake_model{wake_functor{*this}};


    explicit constexpr network_ca(scheduler_type& scheduler) : scheduler{scheduler}
        //f([&](time_point* wake, time_point current) { scheduled(wake, current); })
    {

    }

    explicit constexpr network_ca(
        scheduler_type& scheduler,
        address_manager_type& am) :
        scheduler{scheduler},
        address_manager{am}
    {

    }

    // DEBT: I think we'd prefer to do this at constructor, but for now is easier
    // to do a manual start call
    void start(transport_type& t);

    void track(const pdu<pgns::address_claimed>&)
    {
        // TODO: Map incoming CA/SA/NAMEs, probably via some kind of impl associated with
        // SA generation
    }

    /// Is the address in this claimed message the same as the one we intend to use?
    constexpr bool is_contender(const pdu<pgns::address_claimed>& p) const
    {
        return p.source_address() == address_;
    }

    /// In response to a contending incoming address claim, initiate process of
    /// coming up with a new candidate SA
    void evaluate_contender(transport_type& t, const pdu<pgns::address_claimed>& p)
    {
        // TODO: We'll need to emit our own address claimed, cannot claim and
        // perhaps do some requests to see what address we should try for

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
