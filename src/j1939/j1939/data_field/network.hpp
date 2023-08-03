/**
 *
 * Covers 'network management' described in J1939-81
 *
 * References:
 *
 * 1. J1939-21 (DEC2006)
 * 2. J1939-71 (MAR2011)
 * 3. J1939-81 (draft MAY2003)
 * 4. J1939 (OCT2007)
 */
#pragma once

#include "base.hpp"
#include "../NAME/name.h"

namespace embr { namespace j1939 {

namespace spn {

// Maybe relates to SAEsa01
template <>
struct type_traits<spns::address_assignment> :
    internal::address_type_traits_base
{

};

template <>
constexpr descriptor get_descriptor<spns::address_assignment>()
{
    return descriptor{9, 1, 8};
}


}


namespace pgn {

// As per [4] Table A2
template <>
struct traits<pgns::address_claimed> : internal::traits_base
{
    static constexpr const char* name()
    {
        return "Address Claimed";
    }

    static constexpr const char* abbrev()
    {
        return "AC";
    }
};

template <>
struct traits<pgns::commanded_address> : internal::traits_base
{
    static constexpr const char* name()
    {
        return "Commanded Address";
    }

    static constexpr const char* abbrev()
    {
        return "CA";
    }
};


}


//
// "The Cannot Claim Address message is the same PGN as the Address Claimed message but has a source
//  address of 254, the null address" [3] 4.2.2.3
template <class TContainer>
struct data_field<pgns::address_claimed, TContainer> :
    NAME<TContainer>
{
    typedef NAME<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    // A tiny bit of a downcast - explicit forward ctor necessitates explicit one of these
    template <class Container2>
    data_field& operator=(const NAME<Container2>& copy_from)
    {
        base_type::operator=(copy_from);
        return *this;
    }
};

/*
 * Needs transport protocol data transfer ( > 8 bytes ) mode */
template <class TContainer>
struct data_field<pgns::commanded_address, TContainer> :
    NAME<TContainer>
{
    typedef NAME<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    uint8_t source_address() const
    {
        return base_type::template get<spns::address_assignment>();
    }

    void source_address(uint8_t v)
    {
        base_type::template set<spns::address_assignment>(v);
    }

    // A tiny bit of a downcast - explicit forward ctor necessitates explicit one of these
    template <class Container2>
    data_field& operator=(const NAME<Container2>& copy_from)
    {
        base_type::operator=(copy_from);
        return *this;
    }
};


}}