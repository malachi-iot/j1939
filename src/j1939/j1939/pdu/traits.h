#pragma once

namespace embr { namespace j1939 {

namespace internal {

// DEBT: Fix up naming
struct address_type_traits_base
{
    static constexpr uint8_t global = 255;
    static constexpr uint8_t null = 254;

    // EXPERIMENTAL
    static constexpr bool assigned(uint8_t v) { return v < 254; }
};

}

}}