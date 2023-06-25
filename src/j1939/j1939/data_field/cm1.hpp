#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::requested_percent_fan_speed> : internal::measured_type_traits {};

}

}}