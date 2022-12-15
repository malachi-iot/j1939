#pragma once

#include "../ca.h"

#include "../data_field/oel.hpp"
#include "../data_field/lighting_command.hpp"


namespace embr { namespace j1939 {

namespace impl {

// This CA is responsible for listening for oel switches and emitting timed blinker commmands
// DEBT: Not sure what the official name of this ECU is, so calling it 'lighting command'
template <class TTransport, class TScheduler>
struct lighting_command_ca : impl::controller_application<TTransport>
{
    typedef j1939::impl::controller_application<TTransport> base_type;
    using typename base_type::transport_type;
    using typename base_type::frame_type;
    using typename base_type::frame_traits;
    typedef TScheduler scheduler_type;

    typedef transport_traits<transport_type> _transport_traits;

    // DEBT: Instead, expose impl_type directly from sechduler_type
    typedef estd::remove_reference_t<decltype(scheduler_type{}.impl())> scheduler_impl_type;
    typedef typename scheduler_impl_type::time_point time_point;
    typedef estd::experimental::function<void(time_point*, time_point)> function_type;

    scheduler_type& scheduler;
    // DEBT: Make the 'f_off' portion compile time optional.  It burns up a bit more memory
    // for just the delayed off feature
    function_type f, f_off;
    transport_type* t;

    struct
    {
        bool left_flashing;
        bool right_flashing;
        // Keeping distinct in case hazards use other lights too at some point
        bool hazards_flashing;

        bool on;
        bool scheduled;     // Not fully functional

        data_field<pgns::oel> last;

        void reset()
        {
            left_flashing = false;
            right_flashing = false;
            on = false;
        }

    }   state;

    bool is_scheduled() const;

    void schedule()
    {
        state.scheduled = true;
        scheduler.schedule_now(f);
    }

    static constexpr estd::chrono::milliseconds blink_delay()
    { return estd::chrono::milliseconds(500); }

    lighting_command_ca(scheduler_type& scheduler);

    template <pgns pgn>
    inline bool process_incoming(transport_type&, pdu<pgn>) { return false; }

    void prep_lighting_command(pdu<pgns::lighting_command>& p) const;
    void scheduled(time_point* wake, time_point current);

    // distinct schedule responder just for timed off command
    void off_scheduled(time_point* wake, time_point current);

    bool process_incoming(transport_type& t, enum_type<spns::turn_signal_switch> v);
    bool process_incoming(transport_type& t, enum_type<spns::high_low_beam_switch> v);
    bool process_incoming(transport_type& t, enum_type<spns::work_light_switch> v);
    bool process_incoming(transport_type& t, enum_type<spns::main_light_switch> v);

    bool process_incoming(transport_type& t, const pdu<pgns::ccvs>& p);
    bool process_incoming(transport_type& t, const pdu<pgns::oel>& p);
};


}

}}