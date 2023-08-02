#pragma once

#include <estd/functional.h>
#include <estd/queue.h>
#include <estd/span.h>

#include "transport.h"

namespace embr { namespace can {

namespace impl {

template <size_t N, class TFrame = can::reference::transport::frame>
struct loopback_transport
{
    typedef estd::layer1::vector<uint8_t, 8> message_type;

    bus_state state_ = bus_state::online;

    constexpr bus_state state() const { return state_; }
    constexpr bool good() const { return state() == bus_state::online; }
    void one_shot(bool) {}

#if UNUSED
    // DEBT: Use reference frame here instead
    struct frame
    {
        // DEBT: Can't const up can_id
        uint32_t can_id;
        message_type payload;

        frame() = default;

        frame(const frame& copy_from) = default;
        frame(frame&& move_from) = default;

        frame(uint32_t can_id, const uint8_t* data, uint8_t len) : can_id{can_id}
        {
            const uint8_t* c = data;
            const uint8_t* end = data + len;

            for (; c < end; ++c)
                payload.push_back(*c);
        }


        frame(uint32_t can_id, const estd::span<uint8_t>& data) : can_id{can_id}
        {
            const uint8_t* c = data.begin();
            const uint8_t* end = data.end();

            for(; c < end; ++c)
                payload.push_back(*c);

            // DEBT: estd::span has a problem doing this
            //for(const uint8_t c : data)
                //payload.push_back(c);
        }
    };
#endif
    using frame = TFrame;

    estd::detail::function<bool(const frame&)> receive_callback;

    struct item
    {
        frame frame_;
        void* sender_;
        int count_;     // number of receivers to distribute to

        // DEBT: layer1::queue should probably use uninitialized array
        item() = default;

        constexpr item(const frame& f, void* s = nullptr) : frame_{f}, sender_{s} {}
    };

    // where 'sent' messages go to just be read back by 'receive'
    estd::layer1::queue<item, N> queue;

    item* peek()
    {
        if(queue.empty()) return nullptr;

        return &queue.front();
    }

    bool send(const frame& f, void* sender = nullptr)
    {
        const item& emplaced = queue.emplace(f, sender);
        bool dequeue = false;

        if (static_cast<bool>(receive_callback))
            dequeue = receive_callback(emplaced.frame_);

        if (dequeue) queue.pop();

        return true;
    }

    void send(uint32_t id, message_type data)
    {
        send(id, estd::span<uint8_t>(data.data(), data.size()));
    }

    void send(uint32_t id, estd::span<uint8_t> data)
    {
        // DEBT: Doesn't work because vector won't take span as an initializer
        //queue.emplace(id, data);
        //queue.push(message{id, data});

        const item& emplaced = queue.emplace(frame{id, data}, nullptr);
        bool dequeue = true;

        if (static_cast<bool>(receive_callback))
            dequeue = receive_callback(emplaced.frame_);

        if (dequeue) queue.pop();
    }

    // DEBT: Be very careful with this, message* is valid only for a short time until someone
    // overwrites that queue slot again.  Will need revision for production use
    const frame* receive()
    {
        if (queue.empty()) return nullptr;

        const frame& front = queue.front().frame_;

        queue.pop();

        return &front;
    }

    bool receive(frame* f)
    {
        if (queue.empty()) return false;

        *f = queue.front().frame_;

        queue.pop();

        return true;
    }
};


// This holds on to particular sender, and filters out receive by said sender
template <size_t N, class Frame = can::reference::transport::frame>
struct filtered_loopback_transport
{
    using transport_type = loopback_transport<N, Frame>;
    transport_type& transport;
    void* identity;
    using frame = typename transport_type::frame;

    constexpr filtered_loopback_transport(
        transport_type& t,
        void* identity) :
        transport{t},
        identity(identity)
    {}

    bool send(const frame& f)
    {
        return transport.send(f, identity);
    }

    bool receive(frame* f)
    {
        typename transport_type::item* i = transport.peek();

        if(i == nullptr) return false;
        if(i->sender_ == identity) return false;

        return transport.receive(f);
    }
};


}

using loopback_transport = impl::loopback_transport<5>;

}}