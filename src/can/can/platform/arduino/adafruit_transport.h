#pragma once

// NOTE: This is for the seemingly-obsoleted https://github.com/adafruit/arduino-CAN
// and NOT the maintained https://github.com/adafruit/Adafruit_CAN 

#include <CAN.h>

#include <estd/algorithm.h>
#include "../../reference.h"

namespace embr { namespace can {

// Compiles, but not runtime tested
struct adafruit_transport : embr::can::reference::transport
{
    bool send(const frame& f)
    {
        // DEBT: Since this is all j1939 centric, we always send out
        // extended packet.
        bool r = CAN.beginExtendedPacket(f.id, f.dlc);
        //CAN.write(f.payload.data(), f.payload.length());
        for(int i = 0; i < f.dlc; i++)
            r &= CAN.write(f.payload[i]);
        return r &= CAN.endPacket();
    }

    bool receive(frame* f)
    {
        int packetSize = CAN.parsePacket();

        if (packetSize)
        {
            f->id = CAN.packetId();
            f->dlc = packetSize;

            for(int i = 0; i < packetSize; i++)
                f->payload[i] = CAN.read();

            return true;
        }

        return false;
    }

    // mimicing ios pattern, this indicates no bus errors.
    // this lib doesn't seem to indicate error conditions :(
    static constexpr const bool good() { return true; }

    // TODO: See if lib supports one shot mode or not
    void one_shot(bool v) { }
};

}}