#pragma once

#include <estd/ostream.h>

#include "fwd.h"

namespace embr { namespace j1939 {

template <class TContainer, class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
    estd::detail::basic_ostream<TStreambuf, TBase>& out,
    const NAME<TContainer>& name)
{
    out << "ig" << (unsigned)name.industry_group() << ' ';
    out << "vs=" << (unsigned)name.vehicle_system() << ':';
    out << (unsigned)name.vehicle_system_instance() << ' ';
    out << "mc=" << (unsigned)name.manufacturer_code();
    out << estd::hex << name.identity_number();
    return out;
}

}}