#pragma once

#include <estd/ostream.h>

#include "fwd.h"

namespace embr { namespace j1939 {

template <class TContainer, class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
    estd::detail::basic_ostream<TStreambuf, TBase>& out,
    const NAME<TContainer>& name)
{
    // DEBT: Consolidate with below payload_put approach
    out << "ig" << (unsigned)name.industry_group() << ' ';
    out << "vs=" << (unsigned)name.vehicle_system() << ':';
    out << (unsigned)name.vehicle_system_instance() << ' ';
    out << "mc=" << (unsigned)name.manufacturer_code();
    out << estd::hex << name.identity_number();
    return out;
}

namespace internal {

struct NAME_payload_put : estd::internal::ostream_functor_tag
{
    const layer1::NAME& name;

    constexpr explicit NAME_payload_put(const layer1::NAME& payload) : name{payload} {}

    template <class TStreambuf, class TBase>
    void operator()(estd::detail::basic_ostream<TStreambuf, TBase>& out) const;
};

template <>
struct payload_put<pgns::address_claimed> : NAME_payload_put
{
    using base_type = NAME_payload_put;

    ESTD_CPP_FORWARDING_CTOR(payload_put)
};


template <>
struct payload_put<pgns::commanded_address> : NAME_payload_put
{
    using base_type = NAME_payload_put;

    ESTD_CPP_FORWARDING_CTOR(payload_put)
};


}

}}