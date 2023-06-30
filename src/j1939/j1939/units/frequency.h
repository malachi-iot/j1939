#pragma once

#include "base.h"

namespace embr { namespace units {

namespace internal { struct frequency_tag {}; }

template <>
struct traits<internal::frequency_tag>
{
    static constexpr const char* name() { return "hertz"; }
    static constexpr const char* abbrev() { return "Hz"; }
};


}}