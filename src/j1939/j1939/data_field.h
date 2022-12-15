/**
 *
 * References:
 *
 * 1. J1939-21 (REV. DEC2006)
 * 2. - 9. Reserved
 * 10. https://grayhill.github.io/comm_protocols/sae_j1939.html
 * 11. J1939-71 (REV. DEC2003)
 */
#pragma once

#include <estd/algorithm.h>

#include <estd/cstdint.h>

#include <embr/bits/bits.hpp>

#include "pgn/enum.h"
#include "spn/enum.h"

#include "data_field/base.hpp"

#include "fwd.h"

namespace embr { namespace j1939 {


template<pgns pgn, class TContainer>
struct data_field :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    data_field(const uint8_t* copy_from) : base_type(copy_from) {}
};




}}