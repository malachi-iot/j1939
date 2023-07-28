/**
 *
 * References:
 *
 * 1. J1939-81 (draft MAY2003)
 * 2. J1939-21 (DEC2006)
 */
#pragma once

namespace embr { namespace j1939 {

namespace impl {

// Pertains to [1] 5.10
template <class TTransport, class TScheduler, class TAddressManager = void>
struct network_ca;

// Pertains to [2] 5.10
template <class TTransport>
struct transport_protocol_ca;

}

}}

// DEBT: Move feature flags elsewhere

// Turns on (default) [1] 4.4.4.3 manual retry mode
#define FEATURE_EMBR_J1939_AC_COLLISION_MANAGEMENT 1
