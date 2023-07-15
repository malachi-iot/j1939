#pragma once

// DEBT: For 'rand'
// DEBT: Would be nicer to use c++11 proper random number features, but that would be
// a huge chore for AVR
#include <estd/cstdlib.h>


// DEBT: Not quite right namespace, 'impl'
namespace embr { namespace j1939 { namespace impl {

struct random_address_manager
{
    static constexpr bool depleted() { return false; }

    static void encountered(uint8_t) {}

    static uint8_t get_candidate();
};

// DEBT: Right now this only is choosing from the narrow arbitrary address range
// DEBT: Move this guy elsewhere
inline uint8_t random_address_manager::get_candidate()
{
    int v = 128 + (rand() % (248 - 127));   // NOLINT
    return (uint8_t)v;
}


}}}