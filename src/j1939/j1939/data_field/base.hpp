/**
 * References:
 *
 * 1. J1939-71
 */
#pragma once

#include "fwd.h"
#include "../units/fwd.h"

#include <embr/bits/bits.hpp>
#include "../spn.h"

#include "../spn/traits.h"
#include "../spn/units.h"

#if __cpp_concepts
#include <concepts>
#endif

namespace embr { namespace j1939 {

namespace experimental {
struct layer1_tag {};
struct layer2_tag {};
struct layer3_tag {};
}

namespace internal {

// length is processed from bit position towards msb, as per [lost reference]
// binary data is little endian, "least significant byte first" [1] 5.1.2
template <class TContainer, bits::endianness e = bits::little_endian>
struct data_field_base :
    bits::internal::material<e, bits::lsb_to_msb, bits::lsb_to_msb,
        TContainer>
{
    typedef TContainer container_type;

protected:
    typedef bits::internal::material<e, bits::lsb_to_msb, bits::lsb_to_msb,
        TContainer> base_type;

    data_field_base()
    {
        estd::fill(container_type::begin(), container_type::end(), 0xFF);
    }

    explicit data_field_base(const uint8_t* copy_from)
    {
        estd::copy_n(copy_from, 8, container_type::begin());
    }

    explicit data_field_base(experimental::layer2_tag, uint8_t* init_from) : base_type(init_from)
    {

    }

public:
    template <class TInt>
    inline TInt get(spn::descriptor d) const
    {
        return base_type::template get<TInt>(d.bytepos - 1, bits::descriptor{d.bitpos-1, d.length});
    }

    template <class TInt>
    inline void set(spn::descriptor d, TInt v)
    {
        return base_type::template set<TInt>(d.bytepos - 1, bits::descriptor{d.bitpos-1, d.length}, v);
    }

    template <size_t bits>
    inline void set(spn::descriptor d, word<bits> v)
    {
        return base_type::template set<typename word<bits>::type>(
            d.bytepos - 1, bits::descriptor{d.bitpos-1, d.length}, v.cvalue());
    }

    // EXPERIMENTAL, untested
    template <size_t bitpos, unsigned length>
    inline void set_exp(unsigned bytepos, word<length> v)
    {
        return base_type::template set<bitpos, length, typename word<length>::type>(
            bytepos, v.cvalue());
    }

    // Does not attempt to promote the type to enum_type
    template <spns spn_, class TTraits = spn::traits<spn_> >
    inline typename TTraits::int_type get_raw() const
    {
        return get<typename TTraits::int_type>(TTraits::get_descriptor());
    }

    template <spns spn_, class TTraits = spn::traits<spn_>,
        estd::enable_if_t<estd::is_base_of<spn::intrinsic_tag, TTraits>::value, bool> = true>
    inline typename TTraits::int_type get() const
    {
        return get_raw<spn_, TTraits>();
    }

    template <spns spn_, class TTraits = spn::traits<spn_>,
        estd::enable_if_t<!estd::is_base_of<spn::intrinsic_tag, TTraits>::value, bool> = true>
    inline typename TTraits::value_type get() const
    {
        return (typename TTraits::value_type)get<typename TTraits::int_type>(TTraits::get_descriptor());
    }

    template <spns spn_>
    inline void set(typename spn::type_traits<spn_>::int_type v)
    {
        constexpr spn::descriptor d = spn::get_descriptor<spn_>();

        set(d, v);
    }

    template <spns spn_>
    inline void set(typename spn::type_traits<spn_>::enum_type v)
    {
        constexpr spn::descriptor d = spn::get_descriptor<spn_>();

        set(d, (typename spn::type_traits<spn_>::int_type)v);
    }

    template <spns spn_>
    inline void set(typename spn::type_traits<spn_>::slot_traits::type v)
    {
        constexpr spn::descriptor d = spn::get_descriptor<spn_>();

        set(d, (typename spn::type_traits<spn_>::int_type)v.root_count());
    }
};


}

// DEBT: Move this elsewhere, but it does work
#if __cpp_concepts
template <class T>
concept UnitsType = std::derived_from<T, estd::internal::units::unit_base_tag>;

template <class T>
concept PropertyType = UnitsType<T> || std::integral<T> || std::is_enum_v<T>;
#endif

// DEBT: Accepting all inputs for name temporarily as we reduce overall
// unit_type implicit behaviors
#define EMBR_J1939_PROPERTY(name)   \
template <ESTD_CPP_CONCEPT(PropertyType) T>                  \
void name(const T& v)               \
{   \
    base_type::template set<spns::name>(unit_type<spns::name>(v));   \
}   \
    \
unit_type<spns::name> name() const   \
{   \
    return base_type::template get<spns::name>(); \
}


#define EMBR_J1939_PROPERTY_ALIAS(name, alias)   \
void alias(unit_type<spns::name> v)  \
{   \
    base_type::template set<spns::name>(v);   \
}   \
    \
unit_type<spns::name> alias() const   \
{   \
    return base_type::template get<spns::name>(); \
}


#define EMBR_J1939_PROPERTY_INLINE2(name)   \
void name(word<d::name().length> v)  \
{   \
    base_type::set(d::name(), v);   \
}   \
    \
word<d::name().length> name() const   \
{   \
    return base_type::template get<typename word<d::name().length>::type>(d::name()); \
}

#define EMBR_J1939_PROPERTY_INLINE(name, d) \
static constexpr spn::descriptor descriptor_##name() \
{ return d; } \
\
void name(word<(d).length> v)  \
{   \
    base_type::set(d, v);   \
}   \
    \
word<d.length> name() const   \
{   \
    return base_type::template get<typename word<(d).length>::type>(d); \
}


}}
