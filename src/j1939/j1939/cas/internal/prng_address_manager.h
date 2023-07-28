#pragma once

#include "rng.h"

namespace embr { namespace j1939 { namespace internal {

// This particular address manager stays in the strict arbitrary assigned
// range
struct prng_address_manager
{
    using rng_type = psuedo_random_generator&;

    psuedo_random_generator prng;

    rng_type rng() { return prng; }

    static constexpr bool depleted() { return false; }

    static void encountered(uint8_t) {}

    uint8_t peek_candidate() const
    {
        uint8_t v = prng.peek_candidate() % 120;
        return v + 128;
    }

    uint8_t get_candidate()
    {
        const uint8_t v = peek_candidate();
        prng.rotate();
        return v;
    }

    prng_address_manager()
    {
        prng.rotate();
    }
};

}}}
