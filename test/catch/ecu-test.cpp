#include <catch.hpp>

#include <can/fwd.h>

#include <j1939/spn/descriptors.h>
#include <j1939/NAME/name.h>

#include <queue>

namespace embr { namespace can {

// We specifically *do not* use can_id and data_field to test translation to
// transports
struct SyntheticTransport
{
    typedef uint32_t endpoint_type;

    struct message_type
    {
        uint8_t payload_[8];
        uint8_t len_;

        message_type() = default;

        message_type(const uint8_t payload[8])
        {
            std::copy_n(&payload[0], 8, &payload_[0]);
            len_ = 8;
        }
    };

    struct frame
    {
        endpoint_type endpoint;
        message_type message;
    };

    std::queue<frame> send_queue;

    bool send(const frame& f)
    {
        send_queue.push(f);
        return true;
    }

    void send(endpoint_type e, message_type& m)
    {
        send_queue.push(frame{e, m});
    }
};

template <>
struct frame_traits<SyntheticTransport::frame>
{
    using frame = SyntheticTransport::frame;

    static uint8_t* payload(frame& f) { return f.message.payload_; }
    static const uint8_t* payload(const frame& f) { return f.message.payload_; }
    static uint8_t length(const frame& f) { return f.message.len_; }

    static uint32_t id(const frame& f) { return f.endpoint; }

    static frame create(uint32_t id, const uint8_t* data, uint8_t length)
    {
        return frame{id, data};
    }
};

}}

TEST_CASE("ecu")
{
    using namespace embr;

    can::SyntheticTransport t;

    SECTION("NAME")
    {
        j1939::NAME<estd::array<uint8_t, 8> > name;
    }
}