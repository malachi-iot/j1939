#pragma once

#include <estd/ratio.h>

#include "concepts.h"

namespace embr { namespace units {

// Copying the playbook from chrono
namespace internal {

template <class T>
struct passthrough
{
    typedef T int_type;

    constexpr T operator()(T v) const { return v; }
};

template <class Rep, class Period, class Tag,
#if __cpp_concepts
    Adder F = passthrough<Rep> >
#else
    typename F = passthrough<Rep> >
#endif
class unit_base;

template <class Rep, class Period>
struct cast_helper;

}

}}