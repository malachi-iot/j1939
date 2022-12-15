#pragma once

#include "fwd.h"
#include "../units/base.h"

namespace embr { namespace j1939 {

template <class TInt, TInt offset_, class TPeriod = estd::ratio<1> >
struct slot_traits_helper
{
    using period = TPeriod;

    using offset = units::internal::adder<TInt, offset_ * period::den / period::num>;

    // EXPERIMENTAL
    typedef TInt offset_type;
    static constexpr TInt get_offset() { return offset_; }
};

}}