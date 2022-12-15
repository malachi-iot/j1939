/**
 * References
 *
 * 1. J1939-71 (REV. DEC2003) section 5.1.4
 * 2. sae_j_1939_spreadsheet_supported_by_TwidoExtreme.xls
 * 3. J1939-71 (REV. MAR2011)
 */
#pragma once

#include "base.hpp"

#include "../spn/fwd.h"
#include "../spn/traits.h"

namespace embr { namespace j1939 {

namespace spn {

/**
 Permissible values:

"A 4 bit parameter to indicate the selected position of the operator's turn signal switch.

0000   No Turn being signaled
0001   Left Turn to be Flashing
0010   Right turn to be Flashing
0011 - 1101   Reserved
1110   Error (to include both left and right selected simultaneously)
1111   Not available (do not change)"
[2]
*/
template <>
struct type_traits<spns::turn_signal_switch>
{
    typedef uint8_t int_type;

    enum class enum_type : uint8_t
    {
        no_turn_being_signaled = 0,
        left_turn_to_be_flashing = 1,
        right_turn_to_be_flashing = 2,
        error = 0b1110, // NOTE: Hard to tell what [2]'s comment really means
        no_change = 0b1111
    };

    typedef enum_type value_type;
};


/**
 * "A 2 bit parameter to indicate the selected position of the operator's high/low beam select switch.

00  Low Beam Selected
01  High Beam Selected
10  Error
11  Not available (do not change)" [2]
 */
template <>
struct type_traits<spns::high_low_beam_switch>
{
    typedef uint8_t int_type;

    enum class enum_type : uint8_t
    {
        low_beam_selected = 0,
        high_beam_selected = 1,
        error = 2,
        no_change = 3
    };

    typedef enum_type value_type;
};

template <>
constexpr descriptor get_descriptor<spns::high_low_beam_switch>()
{
    return descriptor{2, 7, 2};
}

/*
 * "A 4 bit parameter to indicate the selected position of the operator's work light switch. In Ag applications the work lights are often refereed to as field lights.  In on-highway applications the work lights are often referred to as clearance lights and may or may not be operated by a switch separate from the main light switch.

0000   Off - The position by which the operator selects that none of the work lamps are to be on.
0001   Work Light Combination #1 On - The position by which the operator selects that the lamps in the combination defined as Work Light Combination #1 are to be on.
0010   Work Light Combination #2 On - The position by which the operator selects that the lamps in the combination defined as Work Light Combination #2 are to be on.
0011   Work Light Combination #3 On - The position by which the operator selects that the lamps in the combination defined as Work Light Combination #3 are to be on.
0100   Work Light Combination #4 On - The position by which the operator selects that the lamps in the combination defined as Work Light Combination #4 are to be on.
0101-1101    Reserved
1110   Error
1111   Not available (do not change)" [2]

 */
template <>
struct type_traits<spns::work_light_switch>
{
    typedef uint8_t int_type;

    enum class enum_type : uint8_t
    {
        off = 0,
        combination_1 = 1,
        combination_2 = 2,
        combination_3 = 3,
        combination_4 = 4,
        no_change = 0b1111
    };

    typedef enum_type value_type;
};


/**
"A 4 bit parameter to indicate the selected position of the operator's main light switch.

0000 Off - The position by which the operator selects that none of the lamps are to be on.
0001 Park On - The position by which the operator selects that the park lamps are to be on.
0010 Headlight On - The position by which the operator selects that the headlamps are to be on.
0011 Headlight and Park On - The position by which the operator selects that Both the Headlamps and the Park lamps are to be on.
0100 - 0111   Reserved
1000   Delayed Off - The position by which the operator selects that a certain set of lamps are to come On and then are to be turned Off following a delay time (Operators Desired - Delayed Lamp Off Time).
1001 - 1101  Reserved
1110  Error
1111  Not available (do not change)"
[2]
 */
template <>
struct type_traits<spns::main_light_switch> : internal::type_traits_base<uint8_t>
{
    enum class enum_type
    {
        off,
        park_on,
        headlight_on,
        headlight_and_park_on,
        delayed_off = 0b1000,
        noop = 0b1111
    };

    typedef enum_type value_type;
};

template <>
struct type_traits<spns::hazard_light_switch> : internal::measured_type_traits
{
};


template <>
struct type_traits<spns::operators_desired_delay_lamp_off_time> : internal::type_traits_base<uint16_t>
{
};


template <>
constexpr descriptor get_descriptor<spns::hazard_light_switch>()
{
    return descriptor{2, 5, 2};
}


template <>
constexpr descriptor get_descriptor<spns::work_light_switch>()
{
    return descriptor{1, 1, 4};
}



template <>
constexpr descriptor get_descriptor<spns::main_light_switch>()
{
    return descriptor{1, 5, 4};
};

/**
 Permissible values:

"A 4 bit parameter to indicate the selected position of the operator's turn signal switch.

0000   No Turn being signaled
0001   Left Turn to be Flashing
0010   Right turn to be Flashing
0011 - 1101   Reserved
1110   Error (to include both left and right selected simultaneously)
1111   Not available (do not change)"
[7]
*/
// linked to pgns::oel
template <>
constexpr descriptor get_descriptor<spns::turn_signal_switch>()
{
    return descriptor{2, 1, 4};
}


template <>
constexpr descriptor get_descriptor<spns::operators_desired_delay_lamp_off_time>()
{
    return descriptor{4, 1, 16};
}




}


template<class TContainer>
struct data_field<pgns::oel, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    data_field(const uint8_t* copy_from) : base_type(copy_from) {}

    void delay_off_time(uint16_t seconds)
    {
        base_type::set<spns::operators_desired_delay_lamp_off_time>(seconds);
    }

    uint16_t delay_off_time() const
    {
        return base_type::template get<spns::operators_desired_delay_lamp_off_time>();
    }

    EMBR_J1939_PROPERTY(turn_signal_switch);
    EMBR_J1939_PROPERTY(high_low_beam_switch);
    EMBR_J1939_PROPERTY(main_light_switch);
    EMBR_J1939_PROPERTY(hazard_light_switch);
};

}}