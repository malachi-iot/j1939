#pragma once

#include <j1939/pdu.h>
#include <j1939/data_field/bjm1.hpp>

#define CONFIG_GPIO_BUTTON1 4
#define CONFIG_BUTTON1_LATCHING 1
#define CONFIG_BUTTON1_INVERTED 0


namespace j1939 = embr::j1939;


struct ArduinoJoystickSink
{
    template <class Transport, class PDU>
    inline bool process_incoming(Transport, PDU) { return false; }

#if CONFIG_BUTTON1_LATCHING
#endif

    template <class Transport>
    bool process_incoming(Transport&, j1939::pdu<j1939::pgns::basic_joystick_message_1>& pdu)
    {
        switch(pdu.button1_pressed())
        {
            case j1939::spn::discrete_parameters::on:
                digitalWrite(CONFIG_GPIO_BUTTON1, CONFIG_BUTTON1_INVERTED ? LOW : HIGH);
                break;

            case j1939::spn::discrete_parameters::off:
                digitalWrite(CONFIG_GPIO_BUTTON1, CONFIG_BUTTON1_INVERTED ? HIGH : LOW);
                break;

            default: break;
        }

        //digitalOut(CONFIG_GPIO_BUTTON1, pdu.button1_pressed() == j1939::spn::discrete_parameters::on ?
          //  )
        return true;
    }

    template <class Transport, class Frame>
    constexpr static bool process_incoming_default(Transport, Frame)
    {
        return false;
    }
};