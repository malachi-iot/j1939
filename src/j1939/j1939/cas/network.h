/**
 *
 * References:
 *
 * 1. J1939-81 (draft MAY2003)
 * 2. J1939-21 (DEC2006)
 * 3. AddressResolution.md v0.1
 */
#pragma once

#include <estd/chrono.h>
#include <estd/functional.h>
#include <estd/optional.h>
#include <estd/type_traits.h>

#include <embr/service.h>

#include "../ca.h"
#include "../data_field/network.hpp"
#include "../data_field/transport_protocol.hpp"

#include "internal/rng_address_manager.h"

#include "fwd.h"

namespace embr { namespace j1939 {

namespace impl {

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
#ifdef ESP_PLATFORM
    static constexpr const char* TAG = "network_ca";
#endif

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
        claim_send_error,   ///< Same as 'waiting' but bus/send error occurred [3] 1.1.4
        reclaim_waiting,    ///< Waiting period of 0-153ms preceding re-transmit of claim [1] 4.4.4.3
        bus_off,            ///< FIX: These bus_off states appear to be in conflict with reclaim_waiting
        bus_off_recover,

        // claimed state
        expired,            ///< Claim waiting period expired with no incident, meaning we succeeded

        // claim failed state
        failed,             ///< Root substate when we've given up trying to get SA
        cannot_claim_waiting,   ///< Waiting period of 0-153ms preceding the emit of "cannot claim" [1] 4.2.2.3
    };

    states state = states::unstarted;
    substates substate = substates::unstarted;

    using address_traits = spn::internal::address_type_traits_base;
    using address_type = estd::layer1::optional<uint8_t, address_traits::null>;

protected:
    // TODO: Optimize to use sparse/layer0/layer2 name but not at the exclusion
    // of the edge case where a NAME can be totally changed
    layer1::NAME name_;

    // TODO: Change to non-optional (since state machine handles that)
    // May be claimed, claiming or cannot claim depending on
    // state machine
    address_type address_;

#if UNIT_TESTING
public:
#endif
    template <class TContainer>
    constexpr explicit network_ca_base(const NAME<TContainer>& name) :
        name_{name}
    {}

    template <class TLayer0Name, estd::enable_if_t<estd::is_base_of<
        embr::j1939::layer0::sparse_tag, TLayer0Name>::value, bool> = true>
    explicit network_ca_base(TLayer0Name sparse)
    {
        sparse.populate(name_);
    }

    // [1] Figure D1
    constexpr bool skip_timeout() const
    {
        return (address_ >= 0 && address_ <= 127) ||
            (address_ >= 248 && address_ <= 253);
    }


public:
    const address_type& address() const { return address_; }

    // DEBT: Would like this to work, though perhaps not specifically
    // preferred.  See layer2::NAME in fwd for more details as to
    // why it doesn't work yet
    //const layer2::NAME name() { return { name_.data() }; }
    const layer1::NAME& name() { return name_; }

    using milliseconds = estd::chrono::milliseconds;

    static constexpr milliseconds cannot_claim_address_max_timeout() { return milliseconds(153); }
    // [1.5] 4.4.4.3
    static constexpr milliseconds bus_collision_delay() { return milliseconds(153); }

    static constexpr milliseconds address_claim_timeout() { return milliseconds(250); }
    static constexpr milliseconds request_for_address_claim_timeout()
    {
        return milliseconds(1250);
    }

    constexpr bool arbitrary_address_capable() const
    {
        return name_.arbitrary_address_capable();
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

    // DEBT: Filter by scheduler_type::impl::is_chrono, that is a necessity
    // DEBT: Filter by scheduker_type::impl::tag::Function, that is a
    //       necessity at the moment, though network_ca could be specialized to
    //       operate in other modes such as 'Traditional'

    typedef TAddressManager address_manager_type;

    scheduler_type& scheduler;
    address_manager_type address_manager_;

    address_manager_type& address_manager() { return address_manager_; }

    // DEBT: Instead, expose impl_type directly from sechduler_type
    typedef estd::remove_reference_t<decltype(scheduler.impl())> scheduler_impl_type;
    typedef typename scheduler_impl_type::time_point time_point;
    typedef typename scheduler_impl_type::function_type fn_test;

    using function_type = typename scheduler_impl_type::function_type;
    //typedef impl::experimental::ca_time_helper<scheduler_impl_type> helper;

    // Depending on whether we're claiming or request for claim we'll
    // timeout 250ms or 1250ms.  Also expected but not yet implemented
    // is a pre-send timeout with bus_collision_delay
    // NOTE: We miss old 'last_claim' but this is more efficient
    time_point timeout;

    transport_type* t;

    estd::chrono::milliseconds get_send_claim_defer();

    address_type find_new_address()
    {
        if(address_manager().depleted())
            return {};
        else
            return address_manager().get_candidate();
    }

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
        p.payload() = name_;
        _transport_traits::send(t, p);

    }

    void send_cannot_claim(transport_type& t)
    {
        pdu<pgns::address_claimed> p;

        p.can_id().destination_address(address_traits::global);

        send_cannot_claim(t, p);
    }

    void send_claim(transport_type& t, pdu<pgns::address_claimed>& p, uint8_t sa);

    void send_claim(transport_type& t)
    {
        pdu<pgns::address_claimed> p;

        send_claim(t, p, *address_);
    }

    // Emits address claim over transport and assures a followup of
    // is scheduled for 250ms later
    void send_claim_and_schedule(transport_type& t, pdu<pgns::address_claimed>& p, uint8_t sa);

    // DEBT: Need to coordinate this better with 'timeout' assignment,
    // otherwise we'll definitely run into a form of jitter
    bool schedule_address_claim_timeout(time_point* wake)
    {
        if(skip_timeout()) return false;

        *wake += address_claim_timeout();
        return true;
    }


    // Scheduler calls this guy
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

    // EXPERIMENTAL
    template <class TLayer0Name>
    using init1 = embr::j1939::experimental::tuple_init<TLayer0Name, scheduler_type&>;

    // EXPERIMENTAL
    template <class TLayer0Name, estd::enable_if_t<estd::is_base_of<
        embr::j1939::layer0::sparse_tag, TLayer0Name>::value, bool> = true>
    static init1<TLayer0Name> get_init(TLayer0Name sparse, scheduler_type& scheduler)
    {
        // DEBT: For values at a minimum we shouldn't need to forward
        // here
        return init1<TLayer0Name>{ std::forward<TLayer0Name>(sparse), scheduler };
    }

    template <class TLayer0Name, estd::enable_if_t<estd::is_base_of<
        embr::j1939::layer0::sparse_tag, TLayer0Name>::value, bool> = true>
    explicit constexpr network_ca(TLayer0Name sparse,
        scheduler_type& scheduler) :
        network_ca_base(sparse),
        scheduler{scheduler}
    {}

    // EXPERIMENTAL
    template <class TLayer0Name, estd::enable_if_t<estd::is_base_of<
        embr::j1939::layer0::sparse_tag, TLayer0Name>::value, bool> = true>
    explicit constexpr network_ca(init1<TLayer0Name> v) :
        network_ca(estd::get<0>(v), estd::get<1>(v))
    {}

    template <class TContainer>
    explicit constexpr network_ca(const NAME<TContainer>& name,
        scheduler_type& scheduler) :
        network_ca_base(name),
        scheduler{scheduler}
        //f([&](time_point* wake, time_point current) { scheduled(wake, current); })
    {

    }

    template <class TContainer>
    explicit constexpr network_ca(const NAME<TContainer>& name,
        scheduler_type& scheduler,
        address_manager_type& am) :
        network_ca_base(name),
        scheduler{scheduler},
        address_manager_{am}
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

        //state = states::claiming;
        //substate = substates::contending;
    }

    template <pgns pgn>
    inline bool process_incoming(transport_type& t, pdu<pgn> p) { return false; }

    bool process_incoming(transport_type& t, const pdu<pgns::address_claimed>& p);

    bool process_request_for_address_claimed(transport_type& t, const pdu<pgns::request>& p);

    bool process_incoming(transport_type& t, const pdu<pgns::request>& p);
};


}

}}
