/**
 * References
 *
 * 1. J1939-71 (REV. DEV2003)
 */
#pragma once

#include <estd/ratio.h>

#include "enum.h"

namespace embr { namespace j1939 { namespace spn {

namespace internal {

// EXPERIMENTAL
template <unsigned b>
struct sub_byte
{
    static constexpr unsigned bitness = b;
    uint8_t value;
};

/// Looks to be unofficial relative to ranges
/// @tparam bitness
template <unsigned bitness>
struct numeric_traits;

/// Indicates whether this is the j1939 fairly predictable "no action" or "not applicable"
/// message.  Compares to see if all bits are set, which indicates the noop condition
/// @tparam N
/// @tparam TInt
/// @param bitpos - always in lsb_to_msb, 1-based.  Specifying bitpos here lets us do a compile-time
///                 optimization
/// @return
template <unsigned N, class TInt>
constexpr bool noop(TInt, unsigned bitpos);

}

struct descriptor;

/// All values 1-based
struct descriptor
{
    const unsigned bytepos;         ///< 1-based
    const unsigned bitpos;          ///< 1-based
    // DEBT: Redundancy between 'length' and 'type_traits::value_type'.  If we choose to do the
    // word<bits> routine for type_traits, we can eliminate length here altogether even for non-byte
    // boundary scenarios
    const unsigned length;          ///< in bits
};


template <spns spn>
constexpr descriptor get_descriptor();

// Helper for ranges, as per [1] 5.1.4
template <typename TInt>
struct range_traits;

template <spns spn>
struct type_traits;

// Aggregator for all compile-time things we know about an SPN
template <spns spn>
struct traits;

namespace ranges {

// As per [1] 5.1.4
template <typename TInt>
struct valid_signal;

// As per [1] 5.1.4
template <typename TInt>
struct parameter_specific_indicator;

// As per [1] 5.1.4
template <typename TInt>
struct error_indicator;

// As per [1] 5.1.4
template <typename TInt>
struct not_available;

}

// means that type_traits has no special enum_type or other goodies, just a stock standard value_type
struct intrinsic_tag {};

}

// DEBT: value_type might be too generic here - and collides with experimental value_type below,
// but unit_type is slightly misleading
template <spns spn>
using unit_type = typename spn::type_traits<spn>::value_type;

template <spns spn>
using enum_type = typename spn::type_traits<spn>::enum_type;

// EXPERIMENTAL
// DEBT: Move this out of fwd
template <spns spn>
class value_type
{
public:
    typedef spn::traits<spn> traits_type;

private:
    typename traits_type::value_type value;

public:
    value_type(typename traits_type::value_type value) : value{value} {}

    value_type(const value_type&) = default;
};

}}