#pragma once

#include <estd/type_traits.h>

#include "../ca.h"
#include "../pgn/traits.h"

namespace embr { namespace j1939 {

template <class TTransport, class TOStream>
class diagnostic_ca :
    public embr::j1939::impl::controller_application<TTransport>
{
    using base_type = embr::j1939::impl::controller_application<TTransport>;

    using typename base_type::transport_type;
    typedef typename TTransport::frame frame_type;
    typedef can::frame_traits<frame_type> frame_traits;

    using pgns = embr::j1939::pgns;

    template <pgns pgn>
    using pdu = embr::j1939::pdu<pgn>;

    template <class TPDU>
    inline bool process_incoming2(transport_type&, TPDU) { return false; }

    TOStream& out;

public:
    explicit constexpr diagnostic_ca(TOStream& out) : out(out) {}

    // DEBT: inline instead of constexpr seems to hels compiler not favor this
    // one.  However, that is obnoxious
    //template <class TPDU>
    //inline bool process_incoming(transport_type&, TPDU) { return false; }

    bool process_incoming_default(transport_type& t, const frame_type& f) const;    // NOLINT

    template <pgns pgn>
    bool process_incoming(transport_type& t, const pdu<pgn>& p);
};

}}