/**
 *
 * References:
 *
 * 1. J1939-81 (draft MAY2003)
 * 2. J1939-21 (DEC2006)
 */
#pragma once

#include "network.h"

#include "../data_field/request.hpp"

namespace embr { namespace j1939 {

namespace impl {

// See [1] Figure A5, A6, A7
template <class TTransport, class TScheduler>
void network_ca<TTransport, TScheduler>::send_request_for_address_claimed(
    transport_type& t, uint8_t dest)
{
    pdu<pgns::request> p;

    // DEBT: make this pgn param take enum
    p.payload().pgn((uint32_t)pgns::address_claimed);
    p.can_id().source_address(address_traits::null);
    p.can_id().destination_address(dest);

    _transport_traits::send(t, p);
}


template <class TTransport, class TScheduler>
void network_ca<TTransport, TScheduler>::scheduled_claiming(time_point* wake, time_point current)
{
    switch(substate)
    {
        case substates::bus_off:
            // TODO: "Delaying before Address Re-Claim"
            // this is a preceding addition to the regular 250ms timeout
            substate = substates::bus_off_recover;
            // DEBT: Arbitrary delay here, need something way more specific
            *wake += estd::chrono::milliseconds(500);
            break;

        case substates::bus_off_recover:
            send_claim(*t);
            substate = substates::waiting;
            if(!schedule_address_claim_timeout(wake)) // set up 250ms timeout
                // don't wait for scheduling, immediately go to 'waiting' finish portion
                scheduled_claiming(wake, current);
            break;

        case substates::contending:
            break;

        case substates::request_waiting:
            // got to 1.25s timeout for request for address claim
            // [1] Figure A5, A6, A7
            // 1.TODO: For A5, if noone contends, send out claim for X.  Then, wait 250ms for contention
            // 2.TODO: For A5, if someone contends, flow to A6
            // 3.TODO: For A6, use some algorithm to aggregate incoming claimed and
            //       select W and send out claim for W.  Then, wait 250ms for contention
            // 4.TODO: For A7, emit cannot claim address (single address CA)

            // Handles 1 and partial 3
            send_claim(*t);
            substate = substates::waiting;
            // DEBT: Getting here we sorta presume we're arbitrary capable, meaning
            // we always do 250ms wait
            schedule_address_claim_timeout(wake);
            break;

        case substates::waiting:
            // got to timeout without contention means successful claim
            state = states::claimed;
            substate = substates::expired;
            break;

        default:
            break;
    }
}

template <class TTransport, class TScheduler>
bool network_ca<TTransport, TScheduler>::process_incoming(transport_type& t, const pdu<pgns::address_claimed>& p)
{
    pdu<pgns::address_claimed> p_resp;

    uint8_t sa = p.can_id().source_address();
    // we expect all address_claimed messages to be BAM
    //uint8_t da = p.can_id().destination_address();

    switch(state)
    {
        // Incoming address claim after we've settled on our SA.  Evaluate whether
        // we can/should give it up
        case states::claimed:
            if(is_contender(p))
                evaluate_contender(t, p);
            break;

        // Incoming address claim while we're trying to claim SA.  Could be someone
        // specifically contending with our claim
        case states::claiming:
            if(is_contender(p))
                evaluate_contender(t, p);
            break;

        // Incoming address claims after we do a request for address claim is expected.
        // Contention possibility is still present.
        case states::requesting:
            if(is_contender(p))
                evaluate_contender(t, p);

            // TODO: Map incoming CA/SA/NAMEs

            break;

        // If unstarted, we must ignore things until we DO start/init
        // If claim_failed, we've given up trying to get on network
        case states::unstarted:
        case states::claim_failed:
            break;
    }

    // [1] 4.4.3.3
    if(sa == given_address)
    {
        const embr::j1939::layer1::NAME& incoming_name = p.payload();

        // DEBT: a C#-style compare returning an int would be useful here,
        // or the c++ two way compare <=>
        if(name < incoming_name)
        {
            // we have the higher priority name
            // transmit our own address, basically re-announce our claim
            send_claim(t);
        }
        else if(name > incoming_name)
        {
            // we have the lower priority name

            // [1] 4.4.4
            // emit a 'cannot claim' or attempt to claim a new address
            optional_address_type new_address = find_new_address();

            // DEBT: At the moment, find_new_address always finds nothing
            if(new_address.has_value())
            {
                pdu<pgns::address_claimed> p;

                send_claim(t, p, *new_address);

                // TODO: Still need to turn this into 'given_address'
                // if we receive no contention.
            }
            else
                send_cannot_claim(t);
        }
        else
        {
            // equals, which is not a covered scenario that I know of
        }
    }

    return false;
}

template <class TTransport, class TScheduler>
bool network_ca<TTransport, TScheduler>::process_request_for_address_claimed(transport_type& t, const pdu<pgns::request>& p)
{
    // TODO: Verify source address is null and log if it isn't
    uint8_t sa = p.can_id().source_address();

    uint8_t da = p.can_id().destination_address();

    if (!(da == address_traits::global ||
          da == given_address))
        return false;

    switch(state)
    {
        case states::claiming:
            send_claim(t);
            break;

        case states::claimed:
            send_claim(t);
            break;

        case states::claim_failed:
            // [1] 4.4.3.1
            // emit a 'cannot claim'
            send_cannot_claim(t);
            break;

        case states::requesting:
        case states::unstarted:
            return false;
            break;
    }

    return true;
}

template <class TTransport, class TScheduler>
bool network_ca<TTransport, TScheduler>::process_incoming(transport_type& t, const pdu<pgns::request>& p)
{
    switch((pgns)p.payload().pgn())
    {
        // [1] 4.2.1, 4.4.3 - request for address claimed
        case pgns::address_claimed:
        {
            // FIX: Looks like we may have a functional overlap with this
            // and below logic
            process_request_for_address_claimed(t, p);

            //if(given_address.has_value())
            {
                uint8_t sa = p.can_id().source_address();
                uint8_t da = p.can_id().destination_address();

                if (da == address_traits::global ||
                    da == given_address)
                {
                    // TODO: Return 'address_claimed' to requester to let them know indeed someone has this
                    // address

                    // DEBT: This 'optional' approach was nifty, but state machine is better so
                    // stop interrogating has_value
                    if(!given_address.has_value())
                    {
                        if(state == states::claim_failed)
                        {
                            // [1] 4.4.3.1
                            // emit a 'cannot claim'
                            send_cannot_claim(t);
                        }
                        else
                        {
                            // [1] 4.2.1 - "should not send...until an Address Claim has been attempted"

                            // instead, we emit a claim address message of our own
                            uint8_t sa_to_claim = 100;  // FIX: Just an arbitrary number right now

                            pdu<pgns::address_claimed> p_resp;

                            p_resp.can_id().destination_address(address_traits::global);
                            p_resp.payload() = name;
                            p_resp.can_id().source_address(sa_to_claim);

                            _transport_traits::send(t, p_resp);
                        }
                    }
                    else
                    {
                        // [1] 4.4.3.1, 4.4.3.2
                        send_claim(t);
                    }
                }

            }
            return false;
        }

        default:
            return false;
    }
}


template <class TTransport, class TScheduler>
void network_ca<TTransport, TScheduler>::start(transport_type& t)
{
    this->t = &t;

    // DEBT: Not 100% right, more like we have an alleged address and
    // the send_claim is to ensure there's no contention
    if(has_address())
    {
        state = states::claiming;
        substate = substates::waiting;
        send_claim(t);
    }
    else
    {
        state = states::requesting;
        substate = substates::request_waiting;
        send_request_for_address_claimed(t);
    }

    function_type f{&wake_model};

    scheduler.schedule(last_claim + address_claim_timeout(), f);
}


}

}}
