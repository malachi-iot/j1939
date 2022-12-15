/**
 * References:
 *
 * 1. J1939-81 (MAY2003)
 * 2. J1939 (OCT2007)
 */
#pragma once

// NOTE: Though accurate, these are not used internally by j1939 library except for synthetic testing

namespace embr { namespace j1939 {

// As per [2] Table B10
enum class manufacturer_codes
{
    reserved = 0,
    allied_signal,
    caterpillar = 8,
    chrysler,
    motorola = 41,
    bosch_usa = 50,
    bosch_gmbh,
    qualcomm = 189,
    trimble = 1856,
};

}}