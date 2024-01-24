/**
 * References
 *
 * 1. J1939-71 (REV. DEV2003) section 5.1.4
 * 2. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 */
#pragma once

#include "fwd.h"
#include "ranges.h"
#include "../slots/fwd.h"
#include "../units/fwd.h"

#include <estd/cstdint.h>

#include <estd/internal/macro/push.h>

// Arduino compensators
// DEBT: Consolidate with estd/embr variety
#ifdef FEATURE_PRAGMA_PUSH_MACRO
#pragma push_macro("word")
#undef word
#endif


// All code here is in support of [1]
namespace embr { namespace j1939 { namespace spn {

template <typename TInt>
struct range_traits
{
    typedef ranges::valid_signal<TInt> valid_signal_type;
    typedef ranges::error_indicator<TInt> error_indicator_type;
    typedef ranges::parameter_specific_indicator<TInt> parameter_specific_type;

    static bool valid_signal(TInt v)
    {
        return v >= valid_signal_type::min() && v <= valid_signal_type::max();
    }

    static bool error(TInt v)
    {
        return v >= error_indicator_type::min() && v <= error_indicator_type::max();
    }

    static bool parameter_specific(TInt v)
    {
        return v >= parameter_specific_type::min() && v <= parameter_specific_type::max();
    }
};


namespace internal {

template <>
struct numeric_traits<2>
{
    static constexpr uint8_t off = 0;
    static constexpr uint8_t err = 2;
    static constexpr uint8_t noop = 3;
};


template <>
struct numeric_traits<4>
{
    static constexpr uint8_t off = 0;
    static constexpr uint8_t err = 0b1110;
    static constexpr uint8_t noop = 0x15;
};


template <>
struct numeric_traits<8>
{
    static constexpr uint8_t off = 0;
    static constexpr uint8_t err = 0xFE;
    static constexpr uint8_t noop = 0xFF;
};

template <>
struct numeric_traits<16>
{
    static constexpr uint16_t off = 0;
    static constexpr uint16_t noop = 0xFFFF;
};

template <unsigned N, typename TInt>
constexpr bool noop(TInt v, unsigned bitpos)
{
    return v == numeric_traits<N>::noop << (bitpos - 1);
}

// Yields matching int_type and value_type
template <class TInt>
struct type_traits_base
{
    typedef TInt int_type;
    typedef int_type value_type;
};

// Overrides value_type with enum_type
template <class TEnum, class TInt = uint8_t>
struct enum_traits_base : type_traits_base<TInt>
{
    typedef TEnum enum_type;
    typedef TEnum value_type;
};

// helper for status command types
using status_type_traits = enum_traits_base<control_commands>;

// helper for measured command types
using measured_type_traits = enum_traits_base<discrete_parameters>;

// TODO: Does a whole lotta nothing right now, but still makes a good placeholder
struct ascii_type_traits
{
    static constexpr char delimiter = '*';
};


/// Reflects both slot_traits as well as slot_traits' contained
/// value_type and int_type
/// @tparam slot
/// @remarks Used to provide a homogenous traits with value_type and int_type
template <slots slot_>
#if __cpp_concepts
    requires SlotTraits<slot_traits<slot_> >
#endif
struct slot_type_traits
{
    static constexpr slots slot = slot_;
    using slot_traits = embr::j1939::slot_traits<slot_>;
    typedef typename slot_traits::type value_type;      ///< Typically an enum or embr::units type by way of SLOT
    typedef typename value_type::root_rep int_type;     ///< Always the root type, typically an unsigned integer
};

// EXPERIMENTAL - keeping in internal namespace for that reason
// Relying quite heavily on the 'h' convention when using slot_traits_helper
// Be careful, because default offset of '0' we don't use 'h' helper and quite possibly
// not an embr::units type (where 'rep' comes from)
template <slots slot>
constexpr typename slot_type_traits<slot>::value_type::rep get_offset()
{
    return slot_type_traits<slot>::slot_traits::h::get_offset();
}


// Maybe relates to SAEsa01
// See J1939-81 (2003) 4.1.2
struct address_type_traits_base : internal::type_traits_base<uint8_t>
{
    static constexpr uint8_t global = 255;
    static constexpr uint8_t null = 254;

    // EXPERIMENTAL
    static constexpr bool assigned(uint8_t v) { return v < 254; }
};


}

// TODO: Put a constexpr spn member var in here
template <spns spn>
struct traits :
    type_traits<spn>,
    range_traits<typename type_traits<spn>::int_type>
{
    static constexpr descriptor d = spn::get_descriptor<spn>();
    static constexpr descriptor get_descriptor() { return spn::get_descriptor<spn>(); }

    /// Indicate whether specified value is the "no action" value, which is always
    /// all-bits-set.  Default behavior = pass in value *unshifted* from data stream
    /// @param v
    /// @param autoshift if you have shifted the data to comfortable 0-position yourself, set this to false.
    /// @return
    constexpr static bool noop(typename type_traits<spn>::int_type v, bool autoshift = true)
    {
        return internal::noop<d.length>(v, autoshift ? d.bitpos : 1);
    }

    // EXPERIMENTAL
    template <typename Rep, class Period, class Tag,
        ESTD_CPP_CONCEPT(embr::units::internal::Adder<Rep>) F>
    //constexpr
    static bool noop(embr::units::internal::unit_base<Rep, Period, Tag, F> v)
    {
        using helper = ranges::not_available<Rep>;
        // Presumes we're dealing with an embr::units type
        using unit_type = typename type_traits<spn>::value_type;

        // DEBT: The way we filter here is clumsy.  We'd prefer to do this up at
        // the function header
        static_assert(estd::is_same<decltype(v), unit_type>::value, "Queried type must match slot/unit type");

        Rep rc = v.root_count();

        return helper::is_not_available(rc);
    }
};


/// Generic catch-all - reports 8-bit every time
/// @remarks particular pdu .hpp files are expected to specialize this
template <spns spn>
struct type_traits :
    internal::type_traits_base<uint8_t>,
    intrinsic_tag
{
};


}}}

#include <estd/internal/macro/pop.h>

#ifdef FEATURE_PRAGMA_PUSH_MACRO
#pragma pop_macro("word")
#endif
