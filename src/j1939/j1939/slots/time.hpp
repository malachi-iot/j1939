/**
 * References:
 *
 * 1. J1939-71 (MAR2011)
 * 2. https://en.cppreference.com/w/cpp/header/chrono
 */
#pragma once

#include "slots.h"
// DEBT: Not using chrono because we want to use our offset magic here
//#include <estd/chrono.h>
#include "../units/time.h"

namespace embr { namespace j1939 {


template <>
struct slot_traits<slots::SAEcd01> : spn::ranges::valid_signal<uint8_t>
{
    typedef estd::ratio_multiply<estd::chrono::days::period, estd::ratio<1, 4> > ratio_type;
    using type = embr::units::seconds<uint8_t, ratio_type >;
};


template <>
struct slot_traits<slots::SAEcm01> : spn::ranges::valid_signal<uint8_t>
{
    using type = embr::units::months<uint8_t>;
};


template <>
struct slot_traits<slots::SAEtm04> : spn::ranges::valid_signal<uint8_t>
{
    using type = embr::units::seconds<uint8_t, estd::ratio<1, 4> >;
};


template <>
struct slot_traits<slots::SAEtm05> : spn::ranges::valid_signal<uint16_t>
{
    using type = embr::units::seconds<uint16_t>;
};


template <>
struct slot_traits<slots::SAEtm06> : spn::ranges::valid_signal<uint32_t>
{
    using type = embr::units::seconds<uint32_t>;
};

template <>
struct slot_traits<slots::SAEtm07> : spn::ranges::valid_signal<uint8_t>
{
    using h = slot_traits_helper<int8_t, -125>;
    using type = embr::units::minutes<uint8_t, h::offset>;
};

template <>
struct slot_traits<slots::SAEtm08> : spn::ranges::valid_signal<uint8_t>
{
    using type = embr::units::minutes<uint8_t>;
};

template <>
struct slot_traits<slots::SAEtm10> : spn::ranges::valid_signal<uint8_t>
{
    using h = slot_traits_helper<int8_t, -125>;
    using type = embr::units::hours<uint8_t, h::offset>;
};


template <>
struct slot_traits<slots::SAEtm11> : spn::ranges::valid_signal<uint8_t>
{
    using type = embr::units::hours<uint8_t>;
};

template <>
struct slot_traits<slots::SAEtm12>
{
    using h = slot_traits_helper<int16_t, -32127>;
    using type = embr::units::hours<uint16_t, h::offset>;
};

template <>
struct slot_traits<slots::SAEtm13> : spn::ranges::valid_signal<uint16_t>
{
    // 51.2us
    using type = embr::units::seconds<uint16_t, estd::ratio<512, 10000000> >;
};

template <>
struct slot_traits<slots::SAEtm14> : spn::ranges::valid_signal<uint16_t>
{
    // 0.1hr
    using type = embr::units::seconds<uint16_t, estd::ratio<60 * 6> >;
};

template <>
struct slot_traits<slots::SAEtm15> : spn::ranges::valid_signal<uint16_t>
{
    using type = embr::units::minutes<uint16_t>;
};

template <>
struct slot_traits<slots::SAEtm16> : spn::ranges::valid_signal<uint8_t>
{
    // 0.5 ms/bit
    using type = embr::units::seconds<uint8_t, estd::ratio<1, 2000> >;
};

template <>
struct slot_traits<slots::SAEtm17> : spn::ranges::valid_signal<uint8_t>
{
    using type = embr::units::milliseconds<uint8_t>;
};

template <>
struct slot_traits<slots::SAEtm18> : spn::ranges::valid_signal<uint16_t>
{
    // 0.1 us/bit
    using type = embr::units::seconds<uint16_t, estd::ratio<1, 10000000> >;
};

template <>
struct slot_traits<slots::SAEcy01> : spn::ranges::valid_signal<uint8_t>
{
    using type = embr::units::years<uint8_t>;
};




}}