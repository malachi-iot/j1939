#pragma once

#include <estd/iosfwd.h>

#include "fwd.h"
#include "header.h"

namespace embr { namespace j1939 {

template <class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
    estd::detail::basic_ostream<TStreambuf, TBase>& out,
    const pdu1_header& ph)
{
    out << "SA:" << ph.source_address() << ' ';
    out << "DA:" << ph.destination_address();
    return out;
}


template <class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
    estd::detail::basic_ostream<TStreambuf, TBase>& out,
    const pdu2_header& ph)
{
    out << "SA:" << ph.source_address();
    return out;
}

}}