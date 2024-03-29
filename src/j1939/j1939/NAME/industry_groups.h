/**
 * References:
 *
 * 1. J1939 (OCT2007)
 */
#pragma once

#include <estd/cstdint.h>

namespace embr { namespace j1939 {

// [1] Table B1
enum class industry_groups : uint8_t
{
    global = 0,
    on_highway,
    agricultural_and_forestry,
    construction,
    marine,
    industrial = 5,
    process_control = 5,
    stationary = 5,
    reserved1 = 6,
    reserved2 = 7
};

}}
