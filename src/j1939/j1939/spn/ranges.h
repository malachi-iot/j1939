/**
 * References
 *
 * 1. J1939-71 (DEC2003) section 5.1.4
 * 2. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 * 3. J1939-71 (MAR2011)
 */
#pragma once

#include "fwd.h"
#include "ranges.h"

#include <estd/cstdint.h>

#ifdef FEATURE_PRAGMA_PUSH_MACRO
#pragma push_macro("abs")
#pragma push_macro("max")
#pragma push_macro("min")
#pragma push_macro("word")
#undef abs
#undef max
#undef min
#undef word
#endif

// All code here is in support of [1]
namespace embr { namespace j1939 { namespace spn {

namespace ranges {

template <>
struct valid_signal<uint8_t>
{
    static constexpr uint8_t min() { return 0; }
    /// Very top of unsigned integer valid range.  SPN and/or SLOT may have additional limits
    static constexpr uint8_t max() { return 250; }
};

template <>
struct valid_signal<uint16_t>
{
    static constexpr uint16_t min() { return 0; }

    // [1] Appears to have a typo.  [3] Fixes that up
    /// Very top of unsigned integer valid range.  SPN and/or SLOT may have additional limits
    static constexpr uint16_t max() { return 64255; }
};


template <>
struct valid_signal<uint32_t>
{
    static constexpr uint32_t min() { return 0; }

    /// Very top of unsigned integer valid range.  SPN and/or SLOT may have additional limits
    static constexpr uint32_t max() { return 421108125; }
};


template <>
struct valid_signal<char>
{
    static constexpr char min() { return 1; }
    // NOTE: 5.1.2 and 5.1.3 Specifies that ASCII is in fact ISO Latin 1 which
    // extends up past 127, and is by that measure not a signed char.
    static constexpr char max() { return (char)254; }
};


template <>
struct parameter_specific_indicator<uint8_t>
{
    static constexpr uint8_t min() { return 0xFB; }
    static constexpr uint8_t max() { return 0xFB; }
};


template <>
struct parameter_specific_indicator<uint16_t>
{
    static constexpr uint16_t min() { return 0xFB00; }
    static constexpr uint16_t max() { return 0xFBFF; }
};


template <>
struct parameter_specific_indicator<uint32_t>
{
    static constexpr uint32_t min() { return 0xFB000000; }
    static constexpr uint32_t max() { return 0xFBFFFFFF; }
};


template <>
struct error_indicator<uint8_t>
{
    static constexpr uint8_t min() { return 0xFE; }
    static constexpr uint8_t max() { return 0xFE; }

    static constexpr bool is_error(uint8_t v) { return v == 0xFE; }
};


template <>
struct error_indicator<uint16_t>
{
    static constexpr uint16_t min() { return 0xFE00; }
    static constexpr uint16_t max() { return 0xFEFF; }

    static constexpr bool is_error(uint16_t v) { return v >> 8 == 0xFE; }
};


template <>
struct error_indicator<uint32_t>
{
    static constexpr uint32_t min() { return 0xFE000000; }
    static constexpr uint32_t max() { return 0xFEFFFFFF; }

    static constexpr bool is_error(uint32_t v) { return v >> 24 == 0xFE; }
};


template <>
struct error_indicator<char>
{
    static constexpr char min() { return 0; }
    static constexpr char max() { return 0; }

    static constexpr bool is_error(char v) { return v == 0; }
};


template <>
struct not_available<uint8_t>
{
    static constexpr char min() { return 0xFF; }
    static constexpr char max() { return 0xFF; }

    static constexpr bool is_not_available(uint8_t v)
    {
        return v == 0xFF;
    }
};

template <>
struct not_available<uint16_t>
{
    static constexpr uint16_t min() { return 0xFF00; }
    static constexpr uint16_t max() { return 0xFFFF; }

    static constexpr bool is_not_available(uint16_t v)
    {
        return v >> 8 == 0xFF;
    }
};


template <>
struct not_available<uint32_t>
{
    static constexpr bool is_not_available(uint32_t v)
    {
        return v >> 24 == 0xFF;
    }
};


}

}}}


#ifdef FEATURE_PRAGMA_PUSH_MACRO
#pragma pop_macro("min")
#pragma pop_macro("max")
#pragma pop_macro("abs")
#pragma pop_macro("word")
#endif
