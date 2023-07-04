#pragma once

#include <SPI.h>
#include <mcp2515.h>

#include <estd/algorithm.h>
#include "../../fwd.h"

namespace embr { namespace can {

// NOTE: Not tested at all, just a placeholder for if/when we do get
// SPI MCP2515 online
struct autowp_transport
{
    typedef struct can_frame frame;

    MCP2515 mcp2515;
    // DEBT: Artifact of not-really-HAL CAN layer
    MCP2515::ERROR last_error;

    autowp_transport(int cs) : mcp2515(cs) {}

    bool send(const frame& f)
    {
        MCP2515::ERROR e = mcp2515.sendMessage(&f);

        last_error = e;

        return e == MCP2515::ERROR_OK;
    }

    bool receive(frame* f)
    {
        MCP2515::ERROR e = mcp2515.readMessage(f);

        last_error = e;

        return e == MCP2515::ERROR_OK;
    }
};


template <>
struct frame_traits<struct can_frame>
{
    static struct can_frame create(uint32_t id, const uint8_t* payload, unsigned dlc)
    {
        can_frame f;

        f.can_id = id;
        f.can_dlc = dlc;
        memcpy(f.data, payload, dlc);

        return f;
    }

    // EXPERIMENTAL
    inline static can_frame create(uint32_t id, estd::span<uint8_t> payload)
    {
        return create(id, payload.data(), payload.size()); 
    }

};


}}