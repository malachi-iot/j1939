#pragma once

#include "rng.h"

namespace embr { namespace j1939 { namespace internal {

// This particular address manager stays in the strict arbitrary assigned
// range
struct prng_address_manager
{
    psuedo_random_generator prng;

    uint32_t seed = 0;

    static constexpr bool depleted() { return false; }

    void reset()
    {
        seed = 0;
        rotate();
    }

    // The most pseudo of psuedo random!
    void rotate()
    {
        seed <<= 3;
        seed ^= 123456789;
    }

    static void encountered(uint8_t) {}

    uint8_t peek_candidate()
    {
        unsigned v = seed % 120;
        return (uint8_t)v + 128;
    }

    uint8_t get_candidate()
    {
        const uint8_t v = peek_candidate();
        rotate();
        return v;
    }

    prng_address_manager()
    {
        rotate();
    }
};

}}}
