#pragma once

#include <embr/units/amps.h>
#include <embr/units/volts.h>
#include <embr/units/watts.h>

#include "slots.h"
#include "../spn/ranges.h"

namespace embr { namespace j1939 {


template <>
struct slot_traits<slots::SAEec01>
{
    using h = slot_traits_helper<int16_t, -1600, estd::ratio<1, 20>>;
    using type = embr::units::amps<uint16_t, h::period, h::offset>;
};

template <>
struct slot_traits<slots::SAEec02>
{
    using h = slot_traits_helper<int8_t, -125>;
    using type = embr::units::amps<uint8_t, h::period, h::offset >;
};

template <>
struct slot_traits<slots::SAEev01> : spn::ranges::valid_signal<uint16_t>
{
    using type = embr::units::volts<uint16_t, estd::ratio<5, 100> >;
};

template <>
struct slot_traits<slots::SAEev04> : spn::ranges::valid_signal<uint16_t>
{
    using h = slot_traits_helper<int16_t, -1606, estd::ratio<5, 100>>;
    using type = embr::units::volts<uint16_t, h::period, h::offset>;
};


template <>
struct slot_traits<slots::SAEpw01> : spn::ranges::valid_signal<uint32_t>
{
    using h = slot_traits_helper<int32_t, -2000000000>;
    using type = embr::units::watts<uint32_t, h::period, h::offset>;
};

template <>
struct slot_traits<slots::SAEpw02> : spn::ranges::valid_signal<uint16_t>
{
    using type = embr::units::watts<uint16_t, estd::ratio<2> >;
};


template <>
struct slot_traits<slots::SAEpw03> : spn::ranges::valid_signal<uint16_t>
{
    using type = embr::units::watts<uint16_t, estd::ratio<1000, 2> >;
};


template <>
struct slot_traits<slots::SAEpw04> : spn::ranges::valid_signal<uint16_t>
{
    using type = embr::units::watts<uint16_t, estd::ratio<1, 2> >;
};


}}