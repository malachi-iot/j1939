/**
 *
 * References:
 *
 * 1. J1939-81 (draft MAY2003)
 * 2. J1939-21 (DEC2006)
 * 3. J1939-71 (MAR2011)
 */
#pragma once

#include "lighting.h"

namespace embr { namespace j1939 {

namespace impl {

template <class TTransport, class TScheduler>
lighting_command_ca<TTransport, TScheduler>::lighting_command_ca(scheduler_type& scheduler) :
    scheduler(scheduler),
    f([&](time_point* wake, time_point current) { scheduled(wake, current); }),
    f_off([&](time_point* wake, time_point current) { off_scheduled(wake, current); })
{

}

// DEBT: In fact, we could still be scheduled even if these are all clear because
// scheduler may still have 'f' queued for wake
template <class TTransport, class TScheduler>
bool lighting_command_ca<TTransport, TScheduler>::is_scheduled() const
{
    /*
     * doesn't account for noop
    return
        state.last.turn_signal_switch() ==
            enum_type<spns::turn_signal_switch>::left_turn_to_be_flashing ||
        state.last.turn_signal_switch() ==
            enum_type<spns::turn_signal_switch>::right_turn_to_be_flashing ||
        state.last.hazard_light_switch() ==
            enum_type<spns::hazard_light_switch>::enabled; */
    return state.left_flashing || state.right_flashing || state.hazards_flashing;
}

template <class TTransport, class TScheduler>
void lighting_command_ca<TTransport, TScheduler>::prep_lighting_command(
    pdu<pgns::lighting_command>& p) const
{
    spn::status s = state.on ? spn::status::enable : spn::status::disable;

    if(state.left_flashing || state.hazards_flashing)
    {
        p.payload().left_turn_signal(s);
    }
    if(state.right_flashing || state.hazards_flashing)
    {
        p.payload().right_turn_signal(s);
    }
}

template <class TTransport, class TScheduler>
void lighting_command_ca<TTransport, TScheduler>::off_scheduled(time_point* wake, time_point current)
{
    pdu<pgns::lighting_command> p_light;

    p_light.low_beam_headlight(spn::status::disable);
    p_light.high_beam_headlight(spn::status::disable);
    p_light.front_fog_lights(spn::status::disable);

    _transport_traits::send(*t, p_light);
}

template <class TTransport, class TScheduler>
void lighting_command_ca<TTransport, TScheduler>::scheduled(time_point* wake, time_point current)
{
    pdu<pgns::lighting_command> p;

    // If lighting states no longer demand a scheduled task, we're done
    if(!is_scheduled())
    {
        // TODO: Do this force-off immediately rather than waiting for scheduler
        if(state.on)
        {
            state.on = false;
            prep_lighting_command(p);
            _transport_traits::send(*t, p);
        }

        state.scheduled = false;
        return;
    }

    prep_lighting_command(p);

    _transport_traits::send(*t, p);

    state.on = !state.on;
    *wake += blink_delay();
}

template <class TTransport, class TScheduler>
bool lighting_command_ca<TTransport, TScheduler>::process_incoming(
    transport_type& t, enum_type<spns::turn_signal_switch> v)
{
    using namespace embr::j1939;

    typedef enum_type<spns::turn_signal_switch> enum_type;

    switch(v)
    {
        case enum_type::no_turn_being_signaled:
            state.left_flashing = false;
            state.right_flashing = false;
            break;

        case enum_type::left_turn_to_be_flashing:
            state.left_flashing = true;
            schedule();
            break;

        case enum_type::right_turn_to_be_flashing:
            state.right_flashing = true;
            schedule();
            break;

        default:
            break;
    }

    return false;
}

template <class TTransport, class TScheduler>
bool lighting_command_ca<TTransport, TScheduler>::process_incoming(
    transport_type& t, enum_type<spns::main_light_switch> v)
{
    using namespace embr::j1939;

    typedef enum_type<spns::main_light_switch> enum_type;

    pdu<pgns::lighting_command> p_light;

    switch(v)
    {
        case enum_type::delayed_off:
        {
            time_point wake = scheduler.impl().now();
            wake += estd::chrono::seconds(3);
            scheduler.schedule(wake, f_off);
            return false;
        }

        case enum_type::off:
            p_light.low_beam_headlight(spn::status::disable);
            p_light.high_beam_headlight(spn::status::disable);
            p_light.front_fog_lights(spn::status::disable);
            break;

        case enum_type::park_on:
            // DEBT: Pretty sure we want something other than fog lights
            // for 'park lamps'.
            p_light.front_fog_lights(spn::status::enable);
            break;

        case enum_type::headlight_on:
            p_light.low_beam_headlight(spn::status::enable);
            break;

        case enum_type::headlight_and_park_on:
            p_light.low_beam_headlight(spn::status::enable);
            p_light.front_fog_lights(spn::status::enable);
            break;

        default:
            return false;
    }

    _transport_traits::send(t, p_light);
    return true;
}

// NOTE: This is a rework of what I was starting to do in event-loop.hpp controller_application
template <class TTransport, class TScheduler>
bool lighting_command_ca<TTransport, TScheduler>::process_incoming(
    transport_type& t, const pdu<pgns::oel>& p)
{
    using namespace embr::j1939;

    process_incoming(t, p.payload().main_light_switch());

    switch(p.payload().high_low_beam_switch())
    {
        default:
            break;
    }

    switch(p.payload().hazard_light_switch())
    {
        case spn::measured::enabled:
            if(!is_scheduled())
            {
                state.hazards_flashing = true;
                schedule();
            }

            break;

        case spn::measured::disabled:
            state.hazards_flashing = false;

        default:
            // We only evaluate turn signals if hazard lights are not in operation
            process_incoming(t, p.payload().turn_signal_switch());
            break;
    }

    state.last = p.payload();
    this->t = &t;

    return false;
}

template <class TTransport, class TScheduler>
bool lighting_command_ca<TTransport, TScheduler>::process_incoming(
    transport_type& t, const pdu<pgns::ccvs>& p)
{
    pdu<pgns::lighting_command> p_light;

    switch(p.brake_switch())
    {
        case spn::measured::disabled:
            p_light.left_stop(spn::status::disable);
            p_light.right_stop(spn::status::disable);
            p_light.center_stop(spn::status::disable);
            _transport_traits::send(t, p_light);
            break;

        case spn::measured::enabled:
            p_light.left_stop(spn::status::enable);
            p_light.right_stop(spn::status::enable);
            p_light.center_stop(spn::status::enable);
            _transport_traits::send(t, p_light);
            break;

        default:
            break;
    }

    return false;
}

}

}}