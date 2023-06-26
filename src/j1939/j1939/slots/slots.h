#pragma once

#include "fwd.h"
#include "../units/base.h"

namespace embr { namespace j1939 {

///
/// \tparam TInt integer type (precision) to do runtime offset addition as
/// \tparam offset_
/// \tparam TPeriod
// DEBT: Consider swapping things around so that offset is in its original unencoded
// form by default (i.e. get_offset).
template <class TInt, TInt offset_, class TPeriod = estd::ratio<1> >
struct slot_traits_helper
{
    using period = TPeriod;

    // This 'offset' exposes an 'add' method which will add a specified value to
    // 'offset_', where 'offset' is adjusted by the TPeriod base
    // For example, if we're representing a percentage with resolution 0.05
    // and an offset of -100%, adder will, say for an incoming percent of 20:
    // - expect incoming value of 400, since we're at 0.05 resolution (20% adjusted by 0.5)
    // - add -2000 (-100% adjusted by 0.05) resolution
    // - return -1600, yielding -80%
    using offset = units::internal::adder<TInt, offset_ * period::den / period::num>;

    // EXPERIMENTAL
    typedef TInt offset_type;
    static constexpr TInt get_offset() { return offset_; }

    template <class TInt2, template <class, class, class> class TUnit>
    using unit = TUnit<TInt2, period, offset>;
};

}}