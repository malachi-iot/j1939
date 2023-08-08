#pragma once

#include <estd/ostream.h>

#include <j1939/pdu.h>
#include <j1939/data_field/bjm1.hpp>

#include "conf.h"

extern estd::arduino_ostream cout;

namespace j1939 = embr::j1939;


struct ArduinoJoystickSink
{
    template <class Transport, class PDU>
    inline bool process_incoming(Transport, PDU)
    {
        return false;
    }

#if CONFIG_BUTTON1_LATCHING
    bool on = false;
#endif

    void dwb1(bool v)
    {
        digitalWrite(CONFIG_GPIO_BJM1_BUTTON1_SINK, v);
    }

    template <class Transport>
    bool process_incoming(
        Transport&,
        const j1939::pdu<j1939::pgns::basic_joystick_message_1>& pdu)
    {
        switch(pdu.button1_pressed())
        {
            case j1939::spn::discrete_parameters::on:
#if CONFIG_BUTTON1_LATCHING
                dwb1(on = !on);
#else
                dwb1(CONFIG_BUTTON1_INVERTED ? LOW : HIGH);
#endif
                break;

            case j1939::spn::discrete_parameters::off:
#if CONFIG_BUTTON1_LATCHING
#else
                dwb1(CONFIG_BUTTON1_INVERTED ? HIGH : LOW);
#endif
                break;

            case j1939::spn::discrete_parameters::noop:
                break;

            default:
                cout << F("BJM error") << estd::endl;
                break;
        }

        return true;
    }

    template <class Transport, class Frame>
    static bool process_incoming_default(Transport, Frame)
    {
        return false;
    }
};