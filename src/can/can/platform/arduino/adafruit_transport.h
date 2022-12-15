#pragma once

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
        CAN.beginExtendedPacket(f.id);
        for(int i = 0; i < f.dlc; i++)
            CAN.write(f.payload[i]);
        CAN.endPacket();
        return true;
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
};

}}