/**
 *
 * Covers 'network management' described in J1939-81
 *
 * References:
 *
 * 1. J1939-21 (DEC2006)
 * 2. J1939-71 (MAR2011)
 * 3. J1939-81 (draft MAY2003)
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

template <>
struct traits<pgns::address_claimed> : internal::traits_base
{
    static constexpr const char* name()
    {
        return "Address Claim";
    }

    static constexpr const char* abbrev()
    {
        // DEBT: J1939-81 doesn't seem to indicate what abbrev is for this guy
        return "AC";
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
};


}}