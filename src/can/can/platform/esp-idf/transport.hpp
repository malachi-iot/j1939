#pragma once

#include <estd/algorithm.h>
#include <estd/cstdint.h>
#include <estd/span.h>

#include "../../transport.h"

#include "esp_log.h"
#include "driver/twai.h"

// NOTE: Keep this simple, because IIRC embr has its own flavor of can transport in a branch

namespace embr { namespace can { namespace esp_idf {

// NOTE: Non blocking only partially supported,
// we possibly need to denote a timeout occurred
template <bool block_rx, bool block_tx = block_rx>
struct twai_transport
{
    using frame = twai_message_t;

    bool single_shot_ = false;

    void one_shot(bool v) { single_shot_ = v; }

    // DEBT: I really hate doing this.  I'd rather have a
    // specialized class or error handler so that we can
    // utilize native error codes.  That said, this has
    // the advantage that aggregated transport error codes
    // will match.
    static status translate(esp_err_t e)
    {
        switch(e)
        {
            case ESP_OK:
                return status::ok;

            case ESP_ERR_TIMEOUT:
                return status::timeout;

            // DEBT: Need lots of other codes here

            default:
                return status::fail;
        }
    }


    // mimicing ios pattern, this indicates no bus errors.
    static bool good()
    {
        twai_status_info_t status_info;

        if(twai_get_status_info(&status_info) == ESP_OK)
            return status_info.state == TWAI_STATE_RUNNING;

        return false;
    }

    // FIX: Name collision with enum
    static bus_state _status()
    {
        twai_status_info_t status_info;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            twai_get_status_info(&status_info)
        );

        switch(status_info.state)
        {
            case TWAI_STATE_STOPPED:
                return bus_state::offline;

            case TWAI_STATE_RUNNING:
                return bus_state::online;

            case TWAI_STATE_RECOVERING:
                return bus_state::recovering;

            case TWAI_STATE_BUS_OFF:
                return bus_state::bus_off;

            default:
                return bus_state::unknown;
        }
    }

    static constexpr TickType_t rx_ticks()
    {
        return block_rx ? portMAX_DELAY : 0;
    }

    static constexpr TickType_t tx_ticks()
    {
        return block_tx ? portMAX_DELAY : 0;
    }

    static bool send_ll(const frame& f)
    {
        return ESP_ERROR_CHECK_WITHOUT_ABORT(twai_transmit(&f, tx_ticks())) == ESP_OK;
    }

    bool send(const frame& f)
    {
        if(single_shot_ == false)   return send_ll(f);

        frame copied_f = f;

        copied_f.ss = true;

        return send_ll(copied_f);
    }

    twai_message_t rx_msg;

    // OBSOLETE do not use
    const frame* receive()
    {
        ESP_ERROR_CHECK(twai_receive(&rx_msg, portMAX_DELAY));
        return &rx_msg;
    }

    static bool receive(frame* rx_msg)
    {
        return twai_receive(rx_msg, rx_ticks()) == ESP_OK;
    }
};

}

template <>
struct frame_traits<twai_message_t>
{
    using frame = twai_message_t;

    inline static frame create(uint32_t id, const uint8_t* payload, uint8_t length)
    {
        frame f{.flags=0, .identifier=id, .data_length_code=length, .data={}};

        // DEBT: This really needs to be specified either on the parameter line
        // or similar - putting into 29-bit mode.
        //f.flags = TWAI_MSG_FLAG_EXTD; // Old deprecated way
        f.extd = 1;

        estd::copy_n(payload, length, f.data);

        return f;
    }

    // EXPERIMENTAL
    inline static frame create(uint32_t id, estd::span<uint8_t> payload)
    {
        return create(id, payload.data(), payload.size()); 
    }

    inline static const uint8_t* payload(const twai_message_t& message)
    {
        return message.data;
    }

    inline static uint32_t length(const twai_message_t& message)
    {
        return message.data_length_code;
    }

    inline static uint32_t id(const frame& message)
    {
        return message.identifier;
    }
};

}}