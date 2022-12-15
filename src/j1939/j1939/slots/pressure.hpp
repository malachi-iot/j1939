#pragma once

#include "slots.h"
#include "../units/pascals.h"

namespace embr { namespace j1939 {


template <>
struct slot_traits<slots::SAEpr11>
{
    using type = embr::units::pascals<uint8_t, estd::ratio<5> >;
};


template <>
struct slot_traits<slots::SAEpr13>
{
    using type = embr::units::pascals<uint8_t, estd::ratio<8> >;
};


}}