/**
 * References:
 *
 * 1. J1939-71
 */
#pragma once

#include "fwd.h"

#include <embr/bits/bits.hpp>
#include "../spn.h"

#include "../spn/traits.h"

namespace embr { namespace j1939 {

namespace internal {

// length is processed from bit position towards msb, as per [lost reference]
// binary data is little endian, "least significant byte first" [1] 5.1.2
template <class TContainer, bits::endianness e = bits::little_endian>
struct data_field_base :
    bits::internal::material<e, bits::lsb_to_msb, bits::lsb_to_msb,
        TContainer>
{
    typedef TContainer container_type;
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

// DEBT: Accepting all inputs for name temporarily as we reduce overall
// unit_type implicit behaviors
#define EMBR_J1939_PROPERTY(name)   \
template <class T>                  \
void name(T v)                      \
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


namespace spn {

template <>
struct type_traits<spns::parameter_group_number_rqst> : internal::type_traits_base<uint32_t>
{

};

template<>
constexpr descriptor get_descriptor<spns::parameter_group_number_rqst>()
{
    return descriptor{1, 1, 24};
}

}

template <class TContainer>
struct data_field<pgns::request, TContainer> :
    internal::data_field_base<TContainer>
{
    struct d
    {
        //static constexpr spn::descriptor pgn()
        //{ return spn::descriptor{}}
    };

    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    uint32_t pgn() const
    {
        return base_type::template get<spns::parameter_group_number_rqst>();
    }

    void pgn(uint32_t v)
    {
        return base_type::template set<spns::parameter_group_number_rqst>(v);
    }
};


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