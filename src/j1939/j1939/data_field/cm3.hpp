#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

template<class TContainer>
struct data_field<pgns::cab_message_3, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    data_field(const uint8_t* copy_from) : base_type(copy_from) {}
};

}}
