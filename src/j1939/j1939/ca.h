/**
 *
 * Controller Application (CA)
 * No specific CAs are implemented here.  Rather, this is a kind of base class to build them from
 *
 * References:
 *
 * 1. j1939-81 (MAY2003)
 */
#pragma once

#include <estd/tuple.h>

#include <can/transport.h>

#include "data_field/fwd.h"
#include "pdu.h"
#include "pgn.h"

namespace embr { namespace j1939 {

namespace impl {

/// This is a noop reference type.  Only useful for testing and learning
/// how to make a different impl
/// @tparam TTransport
template <class TTransport>
class controller_application
{
protected:
    typedef TTransport transport_type;
    using frame_type = typename transport_type::frame;
    //using endpoint_type = typename transport_type::endpoint_type;
    //using message_type = typename transport_type::message_type;
    using frame_traits = can::frame_traits<frame_type>;

    using pgns = embr::j1939::pgns;
    using spns = embr::j1939::spns;

public:
    // DEBT: We actually want layer2::data_field here but that one needs work
    // to get the constructors online
    template <pgns pgn>
    using data_field = const embr::j1939::layer1::data_field<pgn>;

    template <pgns pgn>
    constexpr bool process_incoming(transport_type&, pdu<pgn>) const { return false; }

    // Effectively undefined/unhandled CAN frame.  Otherwise, you'll want to add to the switch/data_field mapper
    constexpr bool process_incoming_default(transport_type& t, const frame_type& f) const
    {
        return false;
    }
};


template <class ...TCAs>
class controller_application_aggregator
{
    estd::tuple<TCAs...> child_cas;

    static constexpr std::size_t sz = sizeof...(TCAs);

    // Some guidance from
    // https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple
    // std::apply would come in handy here, but only if we are c++17

    template <std::size_t I = 0, class TTransport, pgns pgn>
    constexpr typename estd::enable_if<I == sz, bool>::type
    _process_incoming(TTransport& t, const pdu<pgn>& p) const
    {
        return false;
    }

    template <std::size_t I = 0, class TTransport, pgns pgn>
    inline typename estd::enable_if<(I < sz), bool>::type
    _process_incoming(TTransport& t, const pdu<pgn>& p)
    {
        bool result = estd::get<I>(child_cas).process_incoming(t, p);
        return result | _process_incoming<I + 1>(t, p);
    }

public:

    template <class TTransport, pgns pgn>
    inline bool process_incoming(TTransport& t, const pdu<pgn>& p)
    {
        return _process_incoming(t, p);
    }

    // Effectively undefined/unhandled CAN frame.  Otherwise, you'll want to add to the switch/data_field mapper
    template <class TTransport>
    constexpr bool process_incoming_default(TTransport& t, const typename TTransport::frame& f) const
    {
        // TODO: Aggregate this too
        return false;
    }
};

}

// DEBT: Strongly consider grabbing transport_type from TImpl rather than
// specifying here
template <class TTransport, class TImpl = impl::controller_application<TTransport> >
class controller_application : public TImpl
{
    typedef TImpl impl_type;

    impl_type& impl() { return *this; }
    const impl_type& impl() const { return *this; }

public:
    typedef TTransport transport_type;
    using frame_type = typename transport_type::frame;

    using frame_traits = can::frame_traits<frame_type>;

    bool process_incoming(transport_type& t, const frame_type& f);

    template <class ...TArgs>
    controller_application(TArgs&&...args) : TImpl(std::forward<TArgs>(args)...)
    {

    }
};


}}