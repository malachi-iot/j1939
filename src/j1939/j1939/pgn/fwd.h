/**
 * References:
 *
 * 1. J1939-21 (DEC 2006)
 */
#pragma once

#include "enum.h"
#include "../pdu/fwd.h"

namespace embr { namespace j1939 {

enum class pgns : uint32_t;

namespace internal {

template <pgns pgn>
struct pgn_put;

}

namespace pgn {

template <pgns>
struct traits;

struct descriptor
{
    const unsigned length;
    const unsigned default_priority;
};

// DEBT: We need to break out get_descriptor and use ranges and other constexpr inspections
// to reveal proper priority, and length itself will naturally vary depending on the specific
// PDU in question - though most seem to be 8.  Spec also permits app level adjustment of
// priorities
template <pgns pgn>
constexpr descriptor get_descriptor()
{
    // "The default for all other informational, proprietary, request, and
    // ACK messages is 6" [1] 5.2.1
    return descriptor{8, 6};
}

// DEBT: ALl these get_descriptor specializations need to be elsewhere


// [1] Section 5.4.1
template <>
constexpr descriptor get_descriptor<pgns::request>()
{
    return descriptor{3, 6};
}

template <>
constexpr descriptor get_descriptor<pgns::commanded_address>()
{
    return descriptor{9, 6};
}


}

}

template <j1939::pgns pgn>
constexpr j1939::internal::pgn_put<pgn> put_pdu(const j1939::pdu<pgn>& pdu_);

}