#pragma once

#include <estd/internal/deduce_fixed_size.h>    // For 'Range'
#include "../pgn/enum.h"

// DEBT: Move these FEATURE_* to a feature area

// When outputting a PGN/PDU code, this will auto emit its actual abbrevation,
// if available
#ifndef FEATURE_EMBR_J1939_OSTREAM_PGN_ABBREV
#define FEATURE_EMBR_J1939_OSTREAM_PGN_ABBREV 1
#endif

// Payload specialized output is nice, but does consume extra ROM.  For
// very constrained devices, turning this off results in generic hex dump
// when out << payload occurs.
#ifndef FEATURE_EMBR_J1939_OSTREAM_FULL_PAYLOAD
#define FEATURE_EMBR_J1939_OSTREAM_FULL_PAYLOAD 1
#endif

#ifndef FEATURE_EMBR_J1939_OSTREAM_FULL_CM1
#define FEATURE_EMBR_J1939_OSTREAM_FULL_CM1 FEATURE_EMBR_J1939_OSTREAM_FULL_PAYLOAD
#endif

namespace embr { namespace j1939 {

namespace internal {

// DEBT: Use estd flavor directly out in code
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
