#pragma once

#include "../../pdu.h"
#include "../../pgn.h"

namespace embr { namespace j1939 {

namespace internal {

// DEBT: Naming perhaps should be more of a 'request_state'
template <class TTransport, class TImpl, class TContext>
struct app_state
{
    TTransport& t;
    TImpl& impl;
    TContext& context;

    template <pgns pgn>
    ESTD_CPP_CONSTEXPR_RET bool invoker(uint32_t id, const uint8_t* payload) const
    {
        return impl.process_incoming(t, pdu<pgn>(id, payload), context);
    }
};


template <class TTransport, class TImpl>
struct app_state<TTransport, TImpl, estd::monostate>
{
    TTransport& t;
    TImpl& impl;

    template <pgns pgn>
    ESTD_CPP_CONSTEXPR_RET bool invoker(uint32_t id, const uint8_t* payload) const
    {
        return impl.process_incoming(t, pdu<pgn>(id, payload));
    }
};


}

template <class TTransport, class TImpl, class TContext>
bool process_incoming(internal::app_state<TTransport, TImpl, TContext> state, const typename TTransport::frame& f)
{
    typedef typename TTransport::frame frame_type;
    typedef can::frame_traits<frame_type> frame_traits;

    // DEBT: Something seems off that 'id' is passed in to every case
    // statement.  Feels wrong - too permissive of a conversion? Feels
    // like invoker got a touch too fancy and perhaps should just take a raw
    // uint for the can id?
    // DEBT: Optimize
    pdu1_header id{frame_traits::id(f)};
    pdu2_header _id{frame_traits::id(f)};

    pgns pgn = (pgns) (id.is_pdu1() ? id.range() : _id.range());

    const uint8_t* payload = frame_traits::payload(f);

    switch(pgn)
    {
        case pgns::auxiliary_input_output_status_1:
            return state.template invoker<pgns::auxiliary_input_output_status_1>(id, payload);

        case pgns::acknowledgement:
            return state.template invoker<pgns::acknowledgement>(id, payload);

        case pgns::address_claimed:
            return state.template invoker<pgns::address_claimed>(id, payload);

        case pgns::basic_joystick_message_1:
            return state.template invoker<pgns::basic_joystick_message_1>(id, payload);

        case pgns::brakes:
            return state.template invoker<pgns::brakes>(id, payload);

        case pgns::cab_illumination_message:
            return state.template invoker<pgns::cab_illumination_message>(id, payload);

        case pgns::cab_message1:
            return state.template invoker<pgns::cab_message1>(id, payload);

        case pgns::cab_message_3:
            return state.template invoker<pgns::cab_message_3>(id, payload);

        case pgns::commanded_address:
            return state.template invoker<pgns::commanded_address>(id, payload);

        case pgns::direct_lamp_control_data1:
            return state.template invoker<pgns::direct_lamp_control_data1>(id, payload);

        case pgns::ecm_information:
            return state.template invoker<pgns::ecm_information>(id, payload);

        case pgns::ecu_identification:
            return state.template invoker<pgns::ecu_identification>(id, payload);

        case pgns::electronic_brake_system1:
            return state.template invoker<pgns::electronic_brake_system1>(id, payload);

        case pgns::external_brake_request:
            return state.template invoker<pgns::external_brake_request>(id, payload);

        case pgns::extended_joystick_message_1:
            return state.template invoker<pgns::extended_joystick_message_1>(id, payload);

        case pgns::fan_drive_1:
            return state.template invoker<pgns::fan_drive_1>(id, payload);

        case pgns::fan_drive_2:
            return state.template invoker<pgns::fan_drive_2>(id, payload);

        case pgns::lighting_data:
            return state.template invoker<pgns::lighting_data>(id, payload);

        case pgns::lighting_command:
            return state.template invoker<pgns::lighting_command>(id, payload);

        // manufacturer-specific.  THere's also mf2 and mf3, but those are a whole pgn
        // range so it makes more sense for those to cascade out to 'default'
        // Note also that if one is careful one can specialize payload for mf0/mf1
        case pgns::mf0:
            return state.template invoker<pgns::mf0>(id, payload);

        case pgns::mf1:
            return state.template invoker<pgns::mf1>(id, payload);

        case pgns::NAME_management_message:
            return state.template invoker<pgns::NAME_management_message>(id, payload);

        case pgns::oel:
            return state.template invoker<pgns::oel>(id, payload);

        case pgns::proprietary_method_identification:
            return state.template invoker<pgns::proprietary_method_identification>(id, payload);

        case pgns::request:
            return state.template invoker<pgns::request>(id, payload);

        case pgns::sensor_electrical_power_1:
            return state.template invoker<pgns::sensor_electrical_power_1>(id, payload);

        case pgns::sensor_electrical_power_2:
            return state.template invoker<pgns::sensor_electrical_power_2>(id, payload);

        case pgns::shutdown:
            return state.template invoker<pgns::shutdown>(id, payload);

        case pgns::software_identification:
            return state.template invoker<pgns::software_identification>(id, payload);

        case pgns::switch_bank_control:
            return state.template invoker<pgns::switch_bank_control>(id, payload);

        case pgns::switch_bank_status:
            return state.template invoker<pgns::switch_bank_status>(id, payload);

        case pgns::time_date:
            return state.template invoker<pgns::time_date>(id, payload);

        case pgns::time_date_adjust:
            return state.template invoker<pgns::time_date_adjust>(id, payload);

        case pgns::tp_cm:
            return state.template invoker<pgns::tp_cm>(id, payload);

        case pgns::tp_dt:
            return state.template invoker<pgns::tp_dt>(id, payload);

        case pgns::vehicle_electrical_power_1:
            return state.template invoker<pgns::vehicle_electrical_power_1>(id, payload);

        case pgns::vehicle_position:
            return state.template invoker<pgns::vehicle_position>(id, payload);

        default:
            // NOTE: Would use the same 'process_incoming' name, but inheritance member hiding
            // motivates us to use a different name so that CA impls don't have to manually specify
            // this method
            return state.impl.process_incoming_default(state.t, f);
    }
}

}}