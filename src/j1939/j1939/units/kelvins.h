#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct kelvins_tag {}; }

template <>
struct traits<internal::kelvins_tag>
{
    static constexpr const char* name() { return "kelvins"; }

    static constexpr si::quantities quanitiy = si::quantities::temperature;
};


}}
