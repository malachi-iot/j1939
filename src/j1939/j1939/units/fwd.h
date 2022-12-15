#pragma once

#include <estd/ratio.h>

namespace embr { namespace units {

// Copying the playbook from chrono
namespace internal {

template <class T>
struct passthrough
{
    constexpr T operator()(T v) const { return v; }
};

template <class Rep, class Period, class Tag, typename F = passthrough<Rep> >
class unit_base;

template <class Rep, class Period>
struct cast_helper;

}

}}