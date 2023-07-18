#pragma once

#include "../pgn/fwd.h"

namespace embr { namespace j1939 {

// DEBT: Pick up one out of estd
namespace internal {

template <bool v>
using Range = estd::internal::Range<v>;

}

// TODO: Consider further specializing these based on transport, so that we can use native
// types to avoid copying and unnecessary allocating.  If so, it makes sense to do that by policy

///
/// @tparam pgn
/// @tparam TPolicy - place where perhaps we can specify underlying storage class preferences, etc
template <pgns pgn, class TPolicy = void, typename = internal::Range<true> >
class pdu;

struct pdu1_header;
struct pdu2_header;

}}
