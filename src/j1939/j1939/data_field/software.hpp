/**
 * References
 *
 * 1. J1939-71 (REV. DEC2003)
 */
#pragma once

#include "base.hpp"

#include "../spn/fwd.h"
#include "../spn/traits.h"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::number_of_software_identification_fields> : 
    internal::measured_type_traits
{
};


template <>
struct type_traits<spns::software_identification> : internal::ascii_type_traits
{
    // TODO: ASCII type
};

}

// NOTE: Not sure we want a data field for software_identification pgn
// because it will far exceed our 32-byte utility limit

}}