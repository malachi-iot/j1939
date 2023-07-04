#pragma once

#include <estd/type_traits.h>

#include <j1939/ca.h>
#include <j1939/pgn/traits.h>


template <class TTransport>
class diagnostic_ca : 
    public embr::j1939::impl::controller_application<TTransport>
{
    using base_type = embr::j1939::impl::controller_application<TTransport>;

    using typename base_type::transport_type;

    using pgns = embr::j1939::pgns;

    template <pgns pgn>
    using pdu = embr::j1939::pdu<pgn>;

    template <class TPDU>
    inline bool process_incoming2(transport_type&, TPDU) { return false; }

    const char* name_;

public:
    // DEBT: inline instead of constexpr seems to hels compiler not favor this
    // one.  However, that is obnoxious
    //template <class TPDU>
    //inline bool process_incoming(transport_type&, TPDU) { return false; }

    template <pgns pgn>
    bool process_incoming(transport_type& t, const pdu<pgn>& p);

    const char* name() { return name_; }
};
