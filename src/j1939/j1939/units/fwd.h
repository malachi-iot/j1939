#pragma once

#include <estd/ratio.h>

#include "concepts.h"

namespace embr { namespace units {

template <class>
struct traits;

// Copying the playbook from chrono
namespace internal {

template <typename TInt, TInt add>
struct subtractor;

template <typename TInt, TInt add>
struct adder;

struct unit_base_tag {};

template <class T>
struct passthrough
{
    typedef T int_type;

    // DEBT: Sloppy, but less sloppy than slapping negative signs everywhere
    using reversal = passthrough<T>;

    constexpr T operator()(T v) const { return v; }

#if __cpp_constexpr >= 201304L   // "relaxed constexpr" (just to make debugging easier)
    // Just for diagnostic, prefer above so that value conversions
    // happen sooner and more visibly
    template <class T2>
    constexpr T operator()(const T2& v) const
    {
        const T converted(v);
        return converted;
    }
#endif
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

namespace si {

template <class>
struct traits;

}

}}
