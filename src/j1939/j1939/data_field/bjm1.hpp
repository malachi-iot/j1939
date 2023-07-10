// Basic Joystick
#pragma once

#include "base.hpp"

#include "../slots.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::joystick1_x_axis_position> :
    internal::slot_type_traits<slots::SAEpc02> {};

template <>
struct type_traits<spns::joystick1_y_axis_position> :
    internal::slot_type_traits<slots::SAEpc02> {};

template <>
struct type_traits<spns::joystick1_button1_pressed_status> :
    internal::measured_type_traits {};

template <>
struct type_traits<spns::joystick1_button2_pressed_status> :
    internal::measured_type_traits {};


template<>
constexpr descriptor get_descriptor<spns::joystick1_x_axis_position>()
{
    return { 1, 7, 10 };
}

template<>
constexpr descriptor get_descriptor<spns::joystick1_y_axis_position>()
{
    return { 3, 7, 10 };
}

template<>
constexpr descriptor get_descriptor<spns::joystick1_button1_pressed_status>()
{
    return { 6, 7, 2 };
}

template<>
constexpr descriptor get_descriptor<spns::joystick1_button2_pressed_status>()
{
    return { 6, 5, 2 };
}

}

namespace pgn {

template <>
struct traits<pgns::basic_joystick_message_1> : internal::traits_base
{
    // DEBT Fix up discrepancy between this and return method flavor.  Take
    // into account which, if either, is more friendly with ROM-based strings
    // (i.e. Arduino's F() macro)
    static constexpr const char name[] = "Basic Joystick Message 1";
    static constexpr const char abbrev[] = "BJM1";

    using spns = internal::spns_list<
        s::joystick1_x_axis_neutral_position_status,
        s::joystick1_x_lever_left_neg_pos_status,
        s::joystick1_x_axis_position,
        s::joystick1_y_axis_position,
        s::joystick1_button4_pressed_status,
        s::joystick1_button3_pressed_status,
        s::joystick1_button2_pressed_status,
        s::joystick1_button1_pressed_status,
        s::joystick1_button8_pressed_status,
        s::joystick1_button7_pressed_status,
        s::joystick1_button6_pressed_status,
        s::joystick1_button5_pressed_status,
        s::joystick1_button12_pressed_status,
        s::joystick1_button11_pressed_status,
        s::joystick1_button10_pressed_status,
        s::joystick1_button9_pressed_status>;
};

}


template<class TContainer>
struct data_field<pgns::basic_joystick_message_1, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    data_field() = default;

    constexpr data_field(const uint8_t* copy_from) : base_type(copy_from) {}

    EMBR_J1939_PROPERTY(joystick1_x_axis_position)
    EMBR_J1939_PROPERTY(joystick1_y_axis_position)
    EMBR_J1939_PROPERTY(joystick1_button1_pressed_status)
    EMBR_J1939_PROPERTY(joystick1_button2_pressed_status)
};

}}