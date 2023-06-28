#pragma once

#include "enum.h"

namespace embr { namespace j1939 {

// Indicates:
// - type = particular embr::units unit type associated with this slot
// - h (informal) - slot type helper
// Requires specialization
template <slots slot>
struct slot_traits;

template <slots slot>
using slot_type = typename slot_traits<slot>::type;

}}