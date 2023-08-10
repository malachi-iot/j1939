#pragma once


#include <estd/ostream.h>

#include <j1939/pdu.h>
#include <j1939/ca.hpp>
#include <j1939/data_field/lighting_command.hpp>
#include <j1939/data_field/lighting_data.hpp>

#include "conf.h"

// This is a dashboard-specific lighting command sink - rather than
// output to actual headlight, etc. this is for indicators in the cabin

struct ArduinoLightingCommandSink : embr::j1939::impl::controller_application_base
{
    using status = embr::j1939::spn::status;

    template <class Transport, class PDU>
    static constexpr bool process_incoming(Transport, PDU) { return false; }

    template <class Transport>
    bool process_incoming(
        Transport&,
        const embr::j1939::pdu<pgns::lighting_command>& pdu)
    {
#if CONFIG_GPIO_LEFT_BLINKER_INDICATOR != -1
        switch(pdu.left_turn_signal())
        {
            case status::enable: break;
            default: break;
        }
#endif
#if CONFIG_GPIO_RIGHT_BLINKER_INDICATOR != -1
        switch(pdu.right_turn_signal())
        {
            case status::enable: break;
            default: break;
        }
#endif

        return false;
    }
};