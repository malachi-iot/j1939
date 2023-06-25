#pragma once

#include "fwd.h"
#include "../units/base.h"

namespace embr { namespace j1939 {

// DEBT: Consider making offset always biggest signed int possible since it's
// always (usually?) compile time resolved and we don't  so far even pick up
// offset_type
// Also
// DEBT: Consider swapping things around so that offset is in its original unencoded
// form by default (i.e. get_offset).
template <class TInt, TInt offset_, class TPeriod = estd::ratio<1> >
struct slot_traits_helper
{
    using period = TPeriod;

    // FIX: Document this and I think we may be backwards here 
    using offset = units::internal::adder<TInt, offset_ * period::den / period::num>;

    // EXPERIMENTAL
    typedef TInt offset_type;
    static constexpr TInt get_offset() { return offset_; }
};

}}