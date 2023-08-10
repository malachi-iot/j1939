#pragma once

#include <estd/ostream.h>

#include <j1939/pdu.h>
#include <j1939/data_field/bjm1.hpp>

#include "conf.h"

extern estd::arduino_ostream cout;

namespace j1939 = embr::j1939;


struct ArduinoLightingCommandSink
{
    template <class Transport, class PDU>
    inline bool process_incoming(Transport, PDU)
    {
        return false;
    }

    template <class Transport>
    bool process_incoming(
        Transport&,
        const j1939::pdu<j1939::pgns::lighting_command>& pdu)
    {
        using status = j1939::spn::status;

#if CONFIG_GPIO_LEFT_BLINKER
        switch(pdu.left_turn_signal())
        {
            case status::enable:
                digitalWrite(CONFIG_GPIO_LEFT_BLINKER, HIGH);
                break;

            case status::disable:
                digitalWrite(CONFIG_GPIO_LEFT_BLINKER, LOW);
                break;

            default: break;
        }
#endif
#if CONFIG_GPIO_RIGHT_BLINKER
        switch(pdu.right_turn_signal())
        {
            case status::enable:
                digitalWrite(CONFIG_GPIO_RIGHT_BLINKER, HIGH);
                break;

            case status::disable:
                digitalWrite(CONFIG_GPIO_RIGHT_BLINKER, LOW);
                break;

            default: break;
        }
#endif
#if CONFIG_GPIO_CENTER_STOP_LIGHT
        switch(pdu.center_stop())
        {
            case status::enable:
                digitalWrite(CONFIG_GPIO_CENTER_STOP_LIGHT, HIGH);
                break;

            case status::disable:
                digitalWrite(CONFIG_GPIO_CENTER_STOP_LIGHT, LOW);
                break;

            default: break;
        }
#endif

        return true;
    }

    template <class Transport, class Frame>
    static bool process_incoming_default(Transport, Frame)
    {
        return false;
    }
};