#pragma once

#include "enum.h"

#if __cpp_concepts
#include <concepts>
#endif

namespace embr { namespace j1939 {

#if __cpp_concepts
template <class T>
concept SlotTraits = requires
{
    typename T::type;
};
#endif

// Indicates:
// - type = particular embr::units unit type associated with this slot
// - h (informal) - slot type helper
// Requires specialization
template <slots slot>
struct slot_traits;

template <slots slot>
using slot_type = typename slot_traits<slot>::type;

namespace slot { inline namespace v1 {

namespace internal {

// Only internal because I am undecided about naming
struct slot_type_tag {};

}

// EXPERIMENTAL

template <slots slot_>
struct unit;

}}

}}