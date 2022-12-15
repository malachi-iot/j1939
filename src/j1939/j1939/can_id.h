/**
 * References:
 *
 * 1. https://www.kvaser.com/about-can/higher-layer-protocols/j1939-introduction/
 */
#pragma once

#include <estd/cstdint.h>

#include <embr/bits/word.hpp>

namespace embr { namespace j1939 {

// as per [1]
class can_id
{
protected:
    using desc = bits::descriptor;

    // EXPERIMENTAL
    struct traits
    {
        typedef bits::experimental::bit_traits<26, 3> priority;
    };

    struct d
    {
        static constexpr desc source_address()  { return desc{0, 8}; }
        static constexpr desc pdu_specific()    { return desc{8, 8}; }
        static constexpr desc pdu_format()      { return desc{16, 8}; }
        static constexpr desc priority()        { return desc{26, 3}; }

        static constexpr desc range_pdu1()      { return desc{16, 10}; }
        static constexpr desc range_pdu2()      { return desc{8, 18}; }
    };

    embr::bits::internal::word<29> value;

public:
    constexpr can_id(uint32_t v) : value{v} {}

    uint8_t source_address() const { return value.get(d::source_address()); }

    constexpr uint8_t pdu_specific() const { return value.get(d::pdu_specific()); }

    constexpr uint8_t pdu_format() const { return value.get(d::pdu_format()); }

    bool data_page() const
    {
        return (value & ((uint32_t)1 << 24)) != 0U;
    }

    bool reserved() const
    {
        return (value & ((uint32_t)1 << 25)) != 0U;
    }

    constexpr traits::priority::word_type priority() const { return value.get(d::priority()); }
    //constexpr traits::priority::word_type priority() const { return value.get<traits::priority>(); }

    /// "A value of 0 has the highest priority." [1]
    // FIX: value.value() now a constexpr temporary - could be viable reworking bit traits
    // to take a value and/or reference --
    //constexpr traits::priority::word_type priority() const
    //{ return traits::priority::get(&value.value()); }

    void pdu_specific(uint8_t v) { value.set(d::pdu_specific(), v); }

    void source_address(uint8_t v) { value.set(d::source_address(), v); }

    void priority(uint8_t v) { value.set(d::priority(), v); }

    constexpr bool is_pdu1() const { return pdu_format() < 240; }

    constexpr operator uint32_t() const { return value.value(); }

    // DEBT: Probably could use a better name
    constexpr uint32_t raw() const { return value.value(); }
};


}}