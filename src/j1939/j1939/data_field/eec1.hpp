/**
 *
 * References:
 *
 * 1. https://www.csselectronics.com/pages/j1939-explained-simple-intro-tutorial
 * 2. https://copperhilltech.com/blog/sae-j1939-message-format-and-interpretation-of-pgns/
 * 3. https://static.nhtsa.gov/odi/tsbs/2018/MC-10141869-9999.pdf
 * 4. https://github.com/DanielMartensson/Open-SAE-J1939
 * 5. https://www.isobus.net/isobus/pGNAndSPN
 * 6. https://lnx.numeralkod.com/wordpress/docs/errors-index/suspect-parameter-numbers-spn/#
 * 7. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 * 8. https://embeddedflakes.com/network-management-in-sae-j1939/
 * 9. https://embeddeduse.com/2020/01/17/introduction-to-the-sae-j1939-standard/
 * 10. https://grayhill.github.io/comm_protocols/sae_j1939.html
 * 11. J1939-71 (REV. DEC2003)
 */
#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::actual_engine_percent_torque_fractional>
{
    typedef estd::ratio<125, 1000> factor;
};

// [9] section "J1939 Broadcast Frames"
// aka "additional torque" [9]
// linked to pgns::electronic_engine_controller_1
template <>
constexpr descriptor get_descriptor<spns::actual_engine_percent_torque_fractional>()
{
    return descriptor{1, 5, 4};
}


// [9] section "J1939 Broadcast Frames"
// linked to pgns::electronic_engine_controller_1
template <>
constexpr descriptor get_descriptor<spns::engine_speed>()
{
    //typedef estd::ratio<125, 1000> factor;

    // FIX: bitpos=0 seems wrong
    return descriptor{4, 0, 16};
}


// [9] section "J1939 Broadcast Frames"
// linked to pgns::electronic_engine_controller_1
template <>
constexpr descriptor get_descriptor<spns::engine_torque_mode>()
{
    return descriptor{1, 1, 4};
}


}

}}