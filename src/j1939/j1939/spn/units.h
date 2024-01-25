#pragma once

#include "enum.h"
#include "fwd.h"
#include "../slots/units.h"

#include <estd/internal/macro/push.h>

namespace embr { namespace j1939 { namespace spn { inline namespace v1 {

template <spns spn_>
struct unit<spn_, 
    estd::enable_if_t<
        estd::is_base_of<
            slot::v1::internal::slot_type_tag,
            spn::traits<spn_>
        >::value
    >
> : slot::unit<spn::traits<spn_>::slot>
{
    using traits = spn::traits<spn_>;
    using base_type = slot::unit<traits::slot>;

    template <class ...Args>
    constexpr unit(Args&&...args) :
        base_type(std::forward<Args>(args)...)
    {}

    static constexpr spns spn() { return spn_; }
};

}}}}

#include <estd/internal/macro/pop.h>
