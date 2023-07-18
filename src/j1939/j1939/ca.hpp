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

#include <estd/variant.h>

// Have to do this here, because otherwise definition (not necessarily instantiation) will lock
// down pdu<> to generic rather than expected specialized varieties
#include "data_field/all.hpp"
#include "cas/internal/dispatcher.hpp"
#include "ca.h"

namespace embr { namespace j1939 {


template <class TTransport, class TImpl, class TContext>
inline bool process_incoming(TImpl& impl, TTransport& t, const typename TTransport::frame& f, TContext& context)
{
    internal::app_state<TTransport, TImpl, TContext> state{t, impl, context};

    return process_incoming(state, f);
}

template <class TTransport, class TImpl>
inline bool process_incoming(TImpl& impl, TTransport& t, const typename TTransport::frame& f)
{
    internal::app_state<TTransport, TImpl, estd::monostate> state{t, impl};

    return process_incoming(state, f);
}

template <class TTransport, class TImpl>
bool controller_application<TTransport, TImpl>::process_incoming(transport_type& t, const frame_type& f)
{
    return j1939::process_incoming(impl(), t, f);
}

}}
