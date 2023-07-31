#pragma once

#include "base.hpp"

#include "../slots.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::auxiliary_temperature_1> : internal::slot_type_traits<slots::SAEtp01>
{
};

}

}}