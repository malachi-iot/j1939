#pragma once

// DEBT: For 'rand'
// DEBT: Would be nicer to use c++11 proper random number features, but that would be
// a huge chore for AVR
#include <estd/cstdlib.h>

// Since we use AVRs sometimes and they don't have c++11's awesome rng stuff, we slum it here.
// Fortunately our RNG needs for J1939 are meager, so collisions and weak randomness are liveable.
// That said, if one gets into a LOT of ECUs (speculating around 20+) the weak randomness may start
// to present issues since it will be less effective at mitigating time and address related collisions

namespace embr { namespace j1939 { namespace internal {

struct psuedo_random_generator
{
    int seed = 0;

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

    constexpr unsigned peek_candidate() const { return seed; }

    unsigned get()
    {
        const unsigned v = peek_candidate();
        rotate();
        return v;
    }
};


struct random_generator
{
    static unsigned get()
    {
        return rand();    // NOLINT
    }
};




}}}