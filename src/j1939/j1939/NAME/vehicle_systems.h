/**
 * References:
 *
 * 1. J1939 (OCT2007)
 */
#pragma once

namespace embr { namespace j1939 {

// [1] Table B12, applies more or less to IG 0, 1 and 2
enum class vehicle_systems
{
    non_specific = 0,                       ///< IG=0, 1, 2, 3
    tractor,                                ///< IG=1, 2
    trailer,                                ///< IG=1
    tillage,                                ///< IG=2
    secondary_tillage,                      ///< IG=2
    planters_seeders,                       ///< IG=2
    transport_trailer = 11,                 ///< IG=2
    powered_auxiliary_devices = 13,         ///< IG=2

    power_management_and_lighting_systems = 30,        ///< IG=4
    steering_systems = 40,                             ///< IG=4
    communication = 70,                                ///< IG=4
    instrumentation_general = 80,                      ///< IG=4
    hvac = 90,                                         ///< IG=4

    ig5_not_available = 127,                ///< IG=5 " can be used until an explicit function has been assigned."
};

}}