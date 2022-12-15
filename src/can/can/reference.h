#pragma once

#include "fwd.h"

#include <estd/span.h>

namespace embr { namespace can {

// This namespace encloses synthetic / reference transport information
// which can be useful for constructing real live ones, or for facilitating
// unit tests
namespace reference {

struct transport
{
    struct frame
    {
        uint32_t id;
        uint8_t dlc;
        uint8_t payload[8];

        frame() = default;

        frame(uint32_t id, uint8_t dlc, const uint8_t* p) :
            id{id}, dlc{dlc}
        {
            estd::copy_n(p, dlc, payload);
        }

        frame(const frame&) = default;

        frame(uint32_t can_id, const estd::span<uint8_t>& data) : id{can_id},
            dlc{(uint8_t)data.size()}
        {
            const uint8_t* c = data.begin();

            estd::copy_n(c, dlc, payload);
        }
    };
};


}

template <>
struct frame_traits<reference::transport::frame>
{
    using frame = reference::transport::frame;

    inline static frame create(uint32_t id, const uint8_t* payload, uint8_t length)
    {
        return frame(id, length, payload);
    }

    constexpr static uint32_t id(const frame& f) { return f.id; }

    constexpr static const uint8_t* payload(const frame& f)
    {
        return f.payload;
    }
};


}}