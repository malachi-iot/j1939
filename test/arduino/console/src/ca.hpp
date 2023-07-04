#pragma once

#include "ca.h"

template <class TTransport>
template <embr::j1939::pgns pgn>
bool diagnostic_ca<TTransport>::process_incoming(transport_type& t, const pdu<pgn>& param_type)
{
    return true;
}