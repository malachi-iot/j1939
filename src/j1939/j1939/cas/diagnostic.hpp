#pragma once

#include "diagnostic.h"

#include "../ostream.h"

namespace embr { namespace j1939 {

// Adapted from https://devblogs.microsoft.com/oldnewthing/20190710-00/?p=102678
// https://stackoverflow.com/questions/39816779/check-if-type-is-defined

// NOTE: They are correct that there are some limitations/problems with this
// is_type_complete


template<typename, typename = void>
struct is_type_complete : estd::false_type {};

template<typename T>
struct is_type_complete<T, estd::enable_if_t<(sizeof(T) > 0)> > : estd::true_type {};

template <class TTransport, class TOStream>
template <embr::j1939::pgns pgn>
bool diagnostic_ca<TTransport, TOStream>::process_incoming(transport_type& t, const pdu<pgn>& p)
{
    out << p << estd::endl;

    return true;
}

}}