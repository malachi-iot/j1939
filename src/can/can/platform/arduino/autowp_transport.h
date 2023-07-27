#pragma once

// https://registry.platformio.org/libraries/autowp/autowp-mcp2515
// https://github.com/autowp/arduino-mcp2515/tree/v1.0.3

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

    // TODO: Would like to access one shot mode (OSM) but feature not yet in
    // the lib https://github.com/autowp/arduino-mcp2515/pull/65 
    void one_shot(bool v) { }

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

    // mimicing ios pattern, this indicates no bus errors.
    constexpr const bool good() const
    {
        // Adapted 'checkError' function, but ERRORMASK is private
        //return (const_cast<MCP2515&>(mcp2515).getErrorFlags() & MCP2515::EFLG_ERRORMASK) == 0; 
        return const_cast<MCP2515&>(mcp2515).checkError() == false;
    }
};


template <>
struct frame_traits<struct can_frame>
{
    using frame = struct can_frame;

    static struct can_frame create(uint32_t id, const uint8_t* payload, unsigned dlc)
    {
        can_frame f;

        // DEBT: Hard wired to extended id for j1939

        f.can_id = id | CAN_EFF_FLAG;
        f.can_dlc = dlc;
        memcpy(f.data, payload, dlc);

        return f;
    }

    // EXPERIMENTAL
    inline static can_frame create(uint32_t id, estd::span<uint8_t> payload)
    {
        return create(id, payload.data(), payload.size()); 
    }

    static constexpr uint32_t id(const frame& f)
    {
        return f.can_id & ~CAN_EFF_FLAG;
    }

    static constexpr const uint8_t* payload(const frame& f)
    {
        return f.data;
    }
};


}}