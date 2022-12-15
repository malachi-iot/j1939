#pragma once

#include <estd/cstdint.h>

namespace embr {

// DEBT: Implement bit endianness as well as byte endianness
// default is big-endian bit order

enum class endianness
{
    big_endian,
    little_endian
};

enum class length_direction
{
    lsb_to_msb,
    msb_to_lsb
};

struct bit_descriptor
{
    const uint8_t bytepos;    ///< 1-based
    const uint8_t bitpos;     ///< 1-based start offset, where '8' represents leftmost/most significant bit
    uint8_t length;     ///< in bits
};


template <endianness, length_direction>
struct bits;


template <typename TInt, length_direction direction>
inline TInt bit_get(bit_descriptor d, const uint8_t* raw);

template <typename TInt, length_direction direction>
inline void bit_set(bit_descriptor d, uint8_t* raw, TInt v);

template <typename TInt, length_direction direction>
inline TInt bit_get_be(bit_descriptor d, const uint8_t* raw);

template <typename TInt, length_direction direction>
inline void bit_set_be(bit_descriptor d, uint8_t* raw, TInt v);

template <typename TInt, length_direction direction>
inline TInt bit_get_le(bit_descriptor d, const uint8_t* raw);

template <typename TInt, length_direction direction>
inline void bit_set_le(bit_descriptor d, uint8_t* raw, TInt v);


template <length_direction direction>
constexpr unsigned lsb_outside_bit_material(unsigned width, bit_descriptor d);

/// Calculates how many lingering lsb bits remain after siphoning out integer word
/// from a uint8_t stream, as described by bit_descriptor.  Likely MSB bit order specific
/// @param width - in bits
/// @param d
/// @return
/// @remark Undefined behavior if width is greater than available bit material
template <>
constexpr unsigned lsb_outside_bit_material<length_direction::msb_to_lsb>(unsigned width, bit_descriptor d)
{
    //unsigned msb_outside_bit_material = 8 - d.bitpos;
    //unsigned lsb_total_bit_material = 16 - msb_outside_bit_material;
    //unsigned lsb_outside_bit_material = lsb_total_bit_material - d.length;

    // (16 - (8 - d.bitpos)) - d.length =
    // (16 + -8 + d.bitpos) - d.length =
    // 16 - 8 + d.bitpos - d.length

    return (width - 8) + d.bitpos - d.length;
}


///
/// @param width - in bits
/// @param d
/// @return
template <>
//constexpr
inline
unsigned lsb_outside_bit_material<length_direction::lsb_to_msb>(unsigned width, bit_descriptor d)
{
    // given
    // { 0b10111111, 0b10000001 } // 0xBF 0x81
    // with d.bitpos = 5, d.bytepos = 1, d.length = 7 and big endian, we have
    // { 0b1011xxxx, 0b100xxxxx }
    // 'x' represents outside bit material, i.e. the bits we are next to the data we're trying to extract
    // in this example, that's 9 total outside bits.  Calling algorithm already accounts for the first byte,
    // so we reduce the bit_material count by 4 - resulting in this case of a return value of 5.

    /*
    unsigned msb_outside_bit_material = 9 - d.bitpos;
    unsigned lsb_total_bit_material_minus_msb = d.length - msb_outside_bit_material;
    unsigned lsb_outside_bit_material = lsb_total_bit_material - d.length;

    return lsb_outside_bit_material; */
    unsigned msb_already_shifted = d.bitpos - 1;
    unsigned remaining_to_shift_pre_lsb = width - msb_already_shifted;
    unsigned lsb_outside_bit_material = remaining_to_shift_pre_lsb - d.length;

    return lsb_outside_bit_material;
}


namespace internal {


// EXPERIMENTAL
// probably not useful
struct bits
{
    template <class TInt>
    static inline TInt get(bit_descriptor d, uint8_t* raw)
    {
        return bit_get<TInt>(d, raw);
    }
};

}

}