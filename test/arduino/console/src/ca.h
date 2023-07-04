#pragma once

#include <j1939/ca.h>

template <class TTransport>
class diagnostic_ca : 
    public embr::j1939::impl::controller_application<TTransport>
{
    using base_type = embr::j1939::impl::controller_application<TTransport>;

    using typename base_type::transport_type;

    // DEBT: inline instead of constexpr seems to hels compiler not favor this
    // one.  However, that is obnoxious
    template <class TPDU>
    inline bool process_incoming(transport_type&, TPDU) { return false; }

    using pgns = embr::j1939::pgns;

    template <pgns pgn>
    using pdu = embr::j1939::pdu<pgn>;

    template <pgns pgn>
    bool process_incoming(transport_type& t, const pdu<pgn>& p);
};
