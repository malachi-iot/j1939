/**
 *
 * NMEA 2000
 *
 * References:
 *
 * 1. https://www.nmea.org/Assets/july%202010%20nmea2000_v1-301_app_b_pgn_field_list.pdf
 */
#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

// aka Binary Status Report [1]
template <class TContainer>
struct data_field<pgns::switch_bank_status, TContainer> : internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    uint8_t device_bank_instance() const
    {
        return base_type::template get<uint8_t>(spn::descriptor{8, 1, 4});
    }

    void device_bank_instance(uint8_t v)
    {
        return base_type::template set<uint8_t>(spn::descriptor{8, 1, 4}, v);
    }

    // Semi-experimental bit accessor
    // DEBT: Put this out into an embr::bits::get_reference or similar
    struct accessor
    {
        uint8_t* const raw;
        const unsigned bitpos;

        operator bool() const
        {
            //const unsigned mask = (1 << bitpos);

            return (*raw & bitpos) != 0;
        }

        accessor operator=(bool v)
        {
            if(v)
                *raw |= (1 << bitpos);
        }
    };

    typedef accessor reference;

    reference operator [](unsigned n)
    {
        unsigned bytepos = n / 8;
        unsigned bitpos = n % 8;
        return reference{base_type::data() + bytepos, bitpos};
    }
};


template <class TContainer>
struct data_field<pgns::switch_bank_control, TContainer> : internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)
};

}}
