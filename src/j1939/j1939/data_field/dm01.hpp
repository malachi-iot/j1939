#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

namespace internal {

struct flash_lamp_traits : internal::type_traits_base<uint8_t>
{
    enum class enum_type : uint8_t
    {
        slow_flash = 0,
        fast_flash,
        reserved,
        do_not_flash
    };
};

}

template <>
struct type_traits<spns::flash_red_stop_lamp> :
    internal::flash_lamp_traits
{
    enum class enum_type : uint8_t {};
};

template<>
constexpr descriptor get_descriptor<spns::flash_red_stop_lamp>()
{
    return { 2, 5, 2 };
}

template<>
constexpr descriptor get_descriptor<spns::failure_mode_identifier>()
{
    return { 5, 1, 5 };
}


}

namespace pgn {

template <>
struct traits<pgns::active_diagnostic_trouble_codes> : internal::traits_base
{
    EMBR_J1939_PGN_TRAIT("DM01", "Active Diagnostic Trouble Codes")

    using spns = internal::spns_list<
        s::protect_lamp,
        s::failure_mode_identifier,
        s::amber_warning_lamp,
        s::red_stop_lamp,
        s::flash_amber_warning_lamp,
        s::flash_red_stop_lamp,
        s::occurrence_count>;
};

}

template<class TContainer>
struct data_field<pgns::active_diagnostic_trouble_codes, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    EMBR_J1939_PROPERTY(flash_red_stop_lamp)
};

}}