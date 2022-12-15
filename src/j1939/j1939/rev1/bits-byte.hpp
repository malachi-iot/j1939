#pragma once

#include "bits.h"

namespace embr {

template <>
inline uint8_t bit_get<uint8_t, length_direction::msb_to_lsb>(bit_descriptor d, const uint8_t* raw)
{
    uint8_t v = raw[d.bytepos - 1];

    const uint8_t mask = (1 << d.bitpos) - 1;

    v &= mask;

    v >>= lsb_outside_bit_material<length_direction::msb_to_lsb>(8, d);

    return v;
}


template <>
inline uint8_t bit_get<uint8_t, length_direction::lsb_to_msb>(bit_descriptor d, const uint8_t* raw)
{
    uint8_t v = raw[d.bytepos - 1];

    v >>= d.bitpos - 1;

    uint8_t mask = 1 << d.length;

    v &= mask - 1;

    return v;
}


template <>
inline bool bit_get<bool, length_direction::lsb_to_msb>(bit_descriptor d, const uint8_t* raw)
{
    d.length = 1;
    return bit_get<uint8_t, length_direction::lsb_to_msb>(d, raw) == 1;
}

template <>
inline void bit_set<uint8_t, length_direction::lsb_to_msb>(bit_descriptor d, uint8_t* raw, uint8_t v)
{
    uint8_t& _v = raw[d.bytepos - 1];

    v <<= d.bitpos - 1;

    uint16_t mask = ((1 << d.length) - 1) << (d.bitpos - 1);

    _v &= ~mask;
    _v |= v;
}

template <>
inline void bit_set<bool, length_direction::lsb_to_msb>(bit_descriptor d, uint8_t* raw, bool v)
{
    d.length = 1;
    bit_set<bool, length_direction::lsb_to_msb>(d, raw, v ? 1 : 0);
}


}