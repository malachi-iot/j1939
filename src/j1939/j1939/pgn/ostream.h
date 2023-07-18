#pragma once

#include <estd/iosfwd.h>
#include <estd/locale.h>

// DEBT: Only need this directly here for 'estd::hex' would be nice if that
// was available in iosfwd instead
#include <estd/ostream.h>

#include "fwd.h"
#include "../pdu/fwd.h"

namespace embr { namespace j1939 {

// DEBT: All this stuff seems better suited to a pdu specific area, not pgn

template <class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
        estd::detail::basic_ostream<TStreambuf, TBase>& out,
        const pdu1_header& ph)
{
}


template <class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
        estd::detail::basic_ostream<TStreambuf, TBase>& out,
        const pdu2_header& ph)
{
}


namespace internal {

template <pgns pgn>
struct payload_put : estd::internal::ostream_functor_tag
{
    const data_field<pgn>& payload;

    constexpr payload_put(const data_field<pgn>& payload) : payload{payload} {}

    template <class TStreambuf, class TBase>
    void operator()(estd::detail::basic_ostream<TStreambuf, TBase>& out) const
    {
        for(unsigned v : payload)   out << v << ' ';
    }
};

template <pgns pgn>
struct pgn_put : estd::internal::ostream_functor_tag
{
    const pdu<pgn>& pdu_;
    using traits = pgn::traits<pgn>;

    constexpr pgn_put(const pdu<pgn>& p) : pdu_{p} {}

    template <class TStreambuf, class TBase>
    void operator()(estd::detail::basic_ostream<TStreambuf, TBase>& out) const
    {
        // DEBT: Consolidate this into a helper function to avoid code bloat
        const char* abbrev = traits::abbrev();
        out << abbrev << ' ' << estd::hex;
        out << "SA:" << pdu_.source_address() << ' ';

        payload_put<pgn>{pdu_.payload()}(out);
    }
};

}

}

template <j1939::pgns pgn>
j1939::internal::pgn_put<pgn> put_pdu(const j1939::pdu<pgn>& pdu_)
{
    return { pdu_ };
}

}