#pragma once

#include <estd/ratio.h>

#include "concepts.h"

namespace embr { namespace units {

template <class>
struct traits;

// Copying the playbook from chrono
namespace internal {

struct unit_base_tag {};

template <class T>
struct passthrough
{
    typedef T int_type;

    constexpr T operator()(T v) const { return v; }
};

// DEBT: Move this elsewhere
#if __cpp_concepts
#define EMBR_J1939_CONCEPT(T) T
#else
#define EMBR_J1939_CONCEPT(T) class
#endif

template <class Rep, class Period, class Tag,
    EMBR_J1939_CONCEPT(Adder<Rep>) = passthrough<Rep> >
class unit_base;

template <class Rep, class Period>
struct cast_helper;

}

}}