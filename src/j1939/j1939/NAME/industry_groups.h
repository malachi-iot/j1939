/**
 * References:
 *
 * 1. J1939 (OCT2007)
 */
#pragma once

namespace embr { namespace j1939 {

enum class industry_groups : uint8_t
{
    global = 0,
    on_highway,
    agricultrual_and_forestry,
    construction,
    marine,
    industrial = 5,
    process_control = 5,
    stationary = 5
};

}}