#pragma once

// DEBT: For 'rand'
// DEBT: Would be nicer to use c++11 proper random number features, but that would be
// a huge chore for AVR
#include <estd/cstdlib.h>

// DEBT: Do this in bitset itself
#include <estd/internal/macro/push.h>
#include <embr/internal/bitset.h>

// DEBT: Not quite right namespace, 'impl'
namespace embr { namespace j1939 { namespace impl {

struct address_tracker2
{
    // Address range 128-247, inclusive
    estd::bitset<15> tracked {};

    static constexpr unsigned range() { return 8; }

    void encountered(uint8_t addr)
    {
        unsigned index = addr - 128;
        index /= range();

        tracked.set(index);
    }

    // Find first 8-element range which is free and mark it as allocated, returning
    // the beginning of that range
    unsigned allocate()
    {
        unsigned start = 128;
        // Not quite 128 available addresses for arbitrary address capable
        for(unsigned i = 0; i < tracked.size(); i++)
        {
            if(!tracked[i])
            {
                // DEBT: Somewhat problematic truly allocating here.  We instead should wait
                // for the claim to be uncontested.  However, in the short term this is OK because
                // either the new CA or the contender will claim this address.
                tracked.set(i);
                return start;
            }

            start += range();
        }

        return 254;
    }

};

struct random_address_manager
{
    // DEBT: Decouple tracker portion from manager somehow
    address_tracker2 tracker;

    static constexpr bool depleted() { return false; }

    void encountered(uint8_t addr)
    {
        tracker.encountered(addr);
    }

    unsigned get_candidate();
};

// DEBT: Right now this only is choosing from the narrow arbitrary address range
// DEBT: Move this guy elsewhere
// DEBT: Return uint8_t, made 'unsigned' just to aid in debugging
inline unsigned random_address_manager::get_candidate()
{
    const unsigned r = rand();    // NOLINT

    if(tracker.tracked.none() || tracker.tracked.all())
    {
        unsigned v = 128 + (r % 120);
        return v;
    }
    else
    {
        const unsigned start = tracker.allocate();

        // return null address denoting failure
        if(start == 254) return 254;

        unsigned v = r % tracker.range();

        return v + start;
    }
}


}}}

#include <estd/internal/macro/pop.h>
