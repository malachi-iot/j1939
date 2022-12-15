#pragma once

#include "bits.h"

namespace embr {

namespace internal {


// c++11 compat way
template <typename TInt>
constexpr bool remainder_bytes_get_be2(TInt& v, const uint8_t* raw, unsigned bytes_to_process)
{
    return
        bytes_to_process > 0 ?
            remainder_bytes_get_be2(
                v = (v << 8) | *++raw,
                raw,
                bytes_to_process - 1) :
            true;
}

template <typename TInt>
inline void remainder_bytes_get_be(TInt& v, const uint8_t* raw, unsigned bytes_to_process)
{
    do
    {
        ++raw;
        v <<= 8;
        v |= *raw;
        --bytes_to_process;
    }
    while(bytes_to_process > 0);
}

}

/*
template <>
inline uint8_t bit_get_be<uint8_t>(bit_descriptor d, const uint8_t* raw)
{
    return bit_get<uint8_t>(d, raw);
}

template <>
inline bool bit_get_be<bool>(bit_descriptor d, const uint8_t* raw)
{
    return bit_get<bool>(d, raw);
}


template <>
inline void bit_set_be<uint8_t>(bit_descriptor d, uint8_t* raw, uint8_t v)
{
    bit_set<uint8_t>(d, raw, v);
}

template <>
inline void bit_set_be<bool>(bit_descriptor d, uint8_t* raw, bool v)
{
    bit_set<bool>(d, raw, v);
}
*/

// bit order=big endian
template <>
inline uint16_t bit_get_be<uint16_t, length_direction::lsb_to_msb>(bit_descriptor d, const uint8_t* raw)
{
    raw += d.bytepos - 1;

    uint16_t v = *raw;

    v >>= d.bitpos - 1;

    internal::remainder_bytes_get_be(v, raw, 1);

    v >>= lsb_outside_bit_material<length_direction::lsb_to_msb>(16, d);

    return v;
}


// bit order=big endian
template <>
inline uint32_t bit_get_be<uint32_t, length_direction::lsb_to_msb>(bit_descriptor d, const uint8_t* raw)
{
    raw += d.bytepos - 1;

    uint32_t v = *raw;

    v >>= d.bitpos - 1;

    internal::remainder_bytes_get_be(v, raw, 3);

    v >>= lsb_outside_bit_material<length_direction::lsb_to_msb>(32, d);

    return v;

}


// bit order=big endian
template <>
inline uint16_t bit_get_be<uint16_t, length_direction::msb_to_lsb>(bit_descriptor d, const uint8_t* raw)
{
    uint16_t v = raw[d.bytepos - 1];

    const uint16_t mask = (1 << d.bitpos) - 1;

    v &= mask;
    v <<= 8;

    // Due to     // https://stackoverflow.com/questions/9860538/unexpected-c-c-bitwise-shift-operators-outcome
    // we can't do a >> then <<, so we manually mask instead
    //v &= 0xFF00;    // strip off lingering bits outside of bitpos

    v |= raw[d.bytepos];

    /* mask already done up above, now we just need to shift based on length
    uint16_t mask = 1 << (d.length + d.bitpos - 1);

    v &= mask - 1; */

    v >>= lsb_outside_bit_material<length_direction::msb_to_lsb>(16, d);

    return v;
}


// bit order=big endian
template <>
inline uint32_t bit_get_be<uint32_t, length_direction::msb_to_lsb>(bit_descriptor d, const uint8_t* raw)
{
    raw += d.bytepos - 1;
    uint32_t v = *raw;

    const uint16_t mask = (1 << d.bitpos) - 1;

    v &= mask;

    internal::remainder_bytes_get_be(v, raw, 3);

    v >>= lsb_outside_bit_material<length_direction::msb_to_lsb>(32, d);

    return v;
}


template <length_direction direction>
struct bits<endianness::big_endian, direction>
{
    template <typename TInt>
    inline static TInt get(bit_descriptor d, const uint8_t* raw)
    {
        return bit_get_be<TInt, direction>(d, raw);
    }
};

}