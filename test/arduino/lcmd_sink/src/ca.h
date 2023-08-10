#pragma once

#include <estd/ostream.h>

#include <j1939/pdu.h>
#include <j1939/data_field/lighting_command.hpp>
#include <j1939/data_field/lighting_data.hpp>

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

    using status = j1939::spn::status;
    using measured = j1939::spn::measured;

    void digital_write(unsigned pin, status);

    j1939::pdu<j1939::pgns::lighting_data> lighting_data_;

    template <class Transport>
    bool process_incoming(
        Transport& transport,
        const j1939::pdu<j1939::pgns::lighting_data>& pdu)
    {
        using traits = j1939::transport_traits<Transport>;

        traits::send(transport, lighting_data_);

        return false;
    }

    template <class Transport>
    bool process_incoming(
        Transport&,
        const j1939::pdu<j1939::pgns::lighting_command>& pdu)
    {
        status s;
#if CONFIG_GPIO_LEFT_BLINKER
        s = pdu.left_turn_signal();
        digital_write(CONFIG_GPIO_LEFT_BLINKER, s);
        lighting_data_.left_turn_signal((measured)s);
#endif
#if CONFIG_GPIO_RIGHT_BLINKER
        s = pdu.right_turn_signal();
        digital_write(CONFIG_GPIO_RIGHT_BLINKER, s);
#endif
#if CONFIG_GPIO_CENTER_STOP_LIGHT
        s = pdu.center_stop();
        digital_write(CONFIG_GPIO_CENTER_STOP_LIGHT, s);
        lighting_data_.center_stop((measured)s);
#endif
        return true;
    }

    template <class Transport, class Frame>
    static bool process_incoming_default(Transport, Frame)
    {
        return false;
    }
};