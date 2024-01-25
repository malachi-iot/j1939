#pragma once

#include "fwd.h"

namespace embr { namespace j1939 {

// DEBT: Move this out of spn namespace
namespace spn { namespace internal {

/// Reflects both slot_traits as well as slot_traits' contained
/// value_type and int_type
/// @tparam slot
/// @remarks Used to provide a homogenous traits with value_type and int_type
template <slots slot_>
#if __cpp_concepts
    requires SlotTraits<slot_traits<slot_> >
#endif
struct slot_type_traits :
    embr::j1939::slot::v1::internal::slot_type_tag
{
    static constexpr slots slot = slot_;
    using slot_traits = embr::j1939::slot_traits<slot_>;
    typedef typename slot_traits::type value_type;      ///< Typically an enum or embr::units type by way of SLOT
    typedef typename value_type::root_rep int_type;     ///< Always the root type, typically an unsigned integer
};

// EXPERIMENTAL - keeping in internal namespace for that reason
// Relying quite heavily on the 'h' convention when using slot_traits_helper
// Be careful, because default offset of '0' we don't use 'h' helper and quite possibly
// not an embr::units type (where 'rep' comes from)
template <slots slot>
constexpr typename slot_type_traits<slot>::value_type::rep get_offset()
{
    return slot_type_traits<slot>::slot_traits::h::get_offset();
}



}}

}}