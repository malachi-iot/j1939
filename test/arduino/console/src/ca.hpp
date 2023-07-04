#pragma once

#include "ca.h"

// Adapted from https://devblogs.microsoft.com/oldnewthing/20190710-00/?p=102678
// https://stackoverflow.com/questions/39816779/check-if-type-is-defined

// NOTE: They are correct that there are some limitations/problems with this
// is_type_complete

template<typename, typename = void>
struct is_type_complete : estd::false_type {};

template<typename T>
struct is_type_complete<T, estd::enable_if_t<(sizeof(T) > 0)> > : estd::true_type {};

template <embr::j1939::pgns, typename = void>
struct traits_wrapper
{
    static constexpr const char specialized = false;

    static const char* name() { return "N/A"; }
};

template <embr::j1939::pgns pgn
    >
struct traits_wrapper<pgn, estd::enable_if_t<
    (sizeof(embr::j1939::pgn::traits<pgn>) > 0)> > :
    embr::j1939::pgn::traits<pgn>
{
    static constexpr const char specialized = true;
};

template <class TTransport>
template <embr::j1939::pgns pgn>
bool diagnostic_ca<TTransport>::process_incoming(transport_type& t, const pdu<pgn>& param_type)
{
    using traits = traits_wrapper<pgn>;

    const char* name = traits::name();

    name_ = name;
    
    //using traits = estd::conditional_t<estd::enable_if_t<
      //  embr::j1939::pgn::traits<pgn>, bool>
    return true;
}