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
#include <estd/internal/variadic.h>

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
    constexpr bool process_incoming_default(transport_type&, const frame_type&) const
    {
        return false;
    }
};

// DEBT: Move aggregator to its own .h/.hpp file
template <class ...TCAs>
class controller_application_aggregator
{
    using tuple = estd::tuple<TCAs...>;

// DEBT
//#if UNIT_TESTING
public:
//#endif
    tuple child_cas;

    template <class TTransport>
    struct visitor
    {
        TTransport& transport;

        template <size_t I, class TCA, pgns pgn>
        bool operator()(estd::variadic::type<I, TCA>, tuple& ccas,
            const pdu<pgn>& p) const
        {
            TCA& ca = estd::get<I>(ccas);

            ca.process_incoming(transport, p);

            return false;
        }

        template <size_t I, class TCA>
        bool operator()(estd::variadic::type<I, TCA>, tuple& ccas,
            const typename TTransport::frame& frame) const
        {
            TCA& ca = estd::get<I>(ccas);

            ca.process_incoming_default(transport, frame);

            return false;
        }
    };

    template <class TTransport, class ...TArgs>
    bool apply(TTransport& transport, TArgs&&...args)
    {
        // DEBT: Pretty sure there's a tuple specific overload of all this
        // and if there isn't, make one.  Specifically, (e)std::apply
        // https://en.cppreference.com/w/cpp/utility/apply.
        // Note though that stock apply doesn't allow for additional
        // parameters - think we might want to make estd::apply do so
        // Note also that existing 'apply' uses a std::move but should
        // use a std::forward
        estd::variadic::type_visitor<TCAs...>::visit(
            visitor<TTransport>{transport}, child_cas,
            std::forward<TArgs>(args)...);

        // DEBT: Need to |= results together, though not 100% sure
        // I like that overall paradigm either
        return false;
    }

public:
    template <class TTransport, pgns pgn>
    bool process_incoming(TTransport& transport, const pdu<pgn>& p)
    {
        return apply(transport, p);
    }

    // Effectively undefined/unhandled CAN frame.  Otherwise, you'll want to add to the switch/data_field mapper
    template <class TTransport>
    bool process_incoming_default(TTransport& transport,
        const typename TTransport::frame& frame)
    {
        return apply(transport, frame);
    }

    controller_application_aggregator() = default;

    template <class ...TArgs>
    constexpr explicit controller_application_aggregator(TArgs&&...args) :
        child_cas(std::forward<TArgs>(args)...)
    {

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

namespace experimental {

template <class ...TArgs>
using tuple_init = estd::tuple<TArgs...>;

}


}}
