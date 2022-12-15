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

template <class TContainer>
struct data_field<pgns::nmea_group_function, TContainer> : internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)
};

}}