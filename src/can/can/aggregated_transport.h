#pragma once

#include <estd/tuple.h>

#include "reference.h"
#include "loopback.h"

namespace embr { namespace can {

template <class... TTransports>
struct aggregated_transport
{
    estd::tuple<TTransports...> transports;

    // NOTE: All frame types must match
    typedef typename estd::remove_reference_t<
        decltype(estd::get<0>(transports))>::frame frame;

    template <size_t idx, estd::enable_if_t<idx == 0, bool> = true>
    constexpr bool _send(const frame& f) { return true; }

    template <size_t idx, estd::enable_if_t<(idx > 0), bool> = true>
    bool _send(const frame& f)
    {
        bool result = estd::get<idx - 1>(transports).send(f);
        return result & _send<idx - 1>(f);
    }

    template <size_t idx, estd::enable_if_t<idx == 0, bool> = true>
    constexpr bool _receive(frame* f, int* i) { return false; }

    template <size_t idx, estd::enable_if_t<(idx > 0), bool> = true>
    bool _receive(frame* f, int* i)
    {
        if(!estd::get<idx - 1>(transports).receive(f))
            return _receive<idx - 1>(f, i);

        if(i != nullptr)    *i = idx - 1;

        return true;
    }

    // returns true if ALL sends succeed, otherwise false
    bool send(const frame& f)
    {
        return _send<sizeof...(TTransports)>(f);

        /*
        estd::apply([&](TTransports&...t)
        {
            t.send(f);
        }, transports); */
    }

    // soft standard is 'idx' of 0 means loopback
    bool receive(frame* f, int* idx = nullptr)
    {
        return _receive<sizeof...(TTransports)>(f, idx);
    }
};

// lift 'frame' from first transport and use it in loopback
template <class TTransport1, class... TTransports>
using loopback_aggregated_transport =
    aggregated_transport<
        can::impl::loopback_transport<5, typename TTransport1::frame>,
        TTransport1, TTransports...>;

}}