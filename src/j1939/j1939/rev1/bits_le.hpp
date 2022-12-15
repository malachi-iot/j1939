#pragma once

#include "bits.h"
#include <estd/numeric.h>

namespace embr {

namespace internal {

//template <class TInt, endianness e, length_direction>
//inline TInt bit_get(bit_descriptor d, const uint8_t* raw);

// For LE processing the last byte needs special treatment, so we don't touch it.
// Therefore, this doesn't touch the first and last bytes
template <typename TInt>
inline void remainder_bytes_get_le(TInt& v, const uint8_t* raw, unsigned bytes_to_process)
{
    for(;--bytes_to_process > 0;)
    {
        v <<= 8;
        v |= raw[bytes_to_process];
    }
}


// TInt must be unsigned of some bit width, otherwise undefined behavior
template <class TInt>
inline TInt bit_get_le_lsb_to_msb(const unsigned bits_to_process, bit_descriptor d, const uint8_t* raw)
{
    constexpr unsigned byte_size = 8;

    // DEBT: Make this unsigned
    unsigned bytes_to_process = bits_to_process / byte_size;
    --bytes_to_process; // zero-based

    raw = raw + d.bytepos - 1;

    TInt v = raw[bytes_to_process];


    const uint16_t mask = (1 << d.bitpos) - 1;

    v &= mask;

    remainder_bytes_get_le(v, raw, bytes_to_process);

    v >>= lsb_outside_bit_material<length_direction::lsb_to_msb>(
        bits_to_process, d);

    return v;
}

// TInt must be unsigned of some bit width, otherwise undefined behavior
template <class TInt>
inline TInt bit_get_le_msb_to_lsb(const unsigned bits_to_process, bit_descriptor d, const uint8_t* raw)
{
    // DEBT: Presumably in 8-bit bytes.  C++ spec calls for us
    // to not assume that though.
    constexpr unsigned byte_size = 8;
    typedef TInt mask_type;
    //typedef uint16_t mask_type;
    unsigned bytes_to_process = bits_to_process / byte_size;
    --bytes_to_process; // zero based

    // position ourselves in the buffer
    raw = raw + d.bytepos - 1;

    // yank the last item out first, since this is little endian
    TInt v = raw[bytes_to_process];

    //const mask_type mask = (1 << d.bitpos) - 1;

    remainder_bytes_get_le(v, raw, bytes_to_process
    /*
    [=](uint8_t r)
    {
        //if(mask == 0) return r;
        if(bits_to_process == d.length || mask == 0) return r;

        return (uint8_t )(r & mask);
    }); */
    );

    v <<= byte_size;

    // trailing outside bit material will be wiped out by shift to the right
    // since this is msb_to_lsb, the data bits are already adjacent
    // to the next byte of data, so no fancy shifts needed there
    unsigned shift_v =
        lsb_outside_bit_material<length_direction::msb_to_lsb>(
        bits_to_process, d);

    v >>= shift_v;

    // strip the lingering outside-length bits - which are easy to mistake for bitpos
    {
        unsigned remaining_outsite_bits = bits_to_process - (d.length + byte_size - d.bitpos);
        const mask_type mask = (1 << byte_size - remaining_outsite_bits) - 1;
        v &= ~mask;
        v |= *raw & mask;
    }

    if(d.length < bits_to_process)
    {
        const mask_type mask = (1 << d.length) - 1;
        v &= mask;
    }

    //if(mask == 0)
        //v >>= d.bitpos - 1;

    return v;
}


}

template <>
inline uint16_t bit_get_le<uint16_t, length_direction::msb_to_lsb>(bit_descriptor d, const uint8_t* raw)
{
    uint16_t v = internal::bit_get_le_msb_to_lsb<uint16_t>(16, d, raw);
#if OLD_WAY_2
    uint16_t v = raw[d.bytepos];

    const uint16_t mask = (1 << d.bitpos) - 1;

    v &= mask;

    v <<= 8;

    v |= raw[d.bytepos - 1];

    v >>= lsb_outside_bit_material<length_direction::msb_to_lsb>(16, d);
#endif

#ifdef OLD_WAY_1
    v <<= 8 - (d.bitpos - 1);
    // Remember, bytepos is 1-based
    v |= raw[d.bytepos - 1];

    // https://stackoverflow.com/questions/9860538/unexpected-c-c-bitwise-shift-operators-outcome
    if(d.length < 16)
    {
        uint16_t mask = 1 << (d.length + d.bitpos - 1);

        v &= mask - 1;
    }

#endif

    return v;
}


template <>
inline uint32_t bit_get_le<uint32_t, length_direction::msb_to_lsb>(bit_descriptor d, const uint8_t* raw)
{
#if OLD_WAY_2
    // Remember, bytepos is 1-based
    uint32_t v = raw[d.bytepos + 2];

    const uint16_t mask = (1 << d.bitpos) - 1;

    v &= mask;
    v <<= 8;
    v |= raw[d.bytepos + 1];
    v <<= 8;
    v |= raw[d.bytepos + 0];
    v <<= 8;
    v |= raw[d.bytepos + -1];

    v >>= lsb_outside_bit_material<length_direction::msb_to_lsb>(32, d);
#endif
    uint32_t v = internal::bit_get_le_msb_to_lsb<uint32_t>(32, d, raw);

    return v;
}


template <length_direction direction>
struct bits<endianness::little_endian, direction>
{
    template <typename TInt>
    inline static void set(bit_descriptor d, uint8_t* raw, TInt v)
    {
        bit_set_le<TInt, direction>(d, raw, v);
    }

    template <typename TInt>
    inline static TInt get(bit_descriptor d, const uint8_t* raw)
    {
        return bit_get_le<TInt, direction>(d, raw);
    }
};


}