#pragma once

#include "ostream.h"
#include "name.h"

namespace embr { namespace j1939 {

namespace internal {

template <class TStreambuf, class TBase>
void NAME_payload_put::operator()(estd::detail::basic_ostream<TStreambuf, TBase>& out) const
{
    out << "ig" << name.industry_group().value() << ' ';
    out << "vs=" << (unsigned)name.vehicle_system() << ':';
    out << name.vehicle_system_instance().value() << ' ';
    out << "f=" << name.function().value() << ':';
    out << name.function_instance().value() << ' ';
    out << "ecu=" << name.ecu_instance().value() << ' ';
    out << "mc=" << name.manufacturer_code().value() << ' ';
    out << "id=" << estd::hex << name.identity_number().value();
}


}

}}