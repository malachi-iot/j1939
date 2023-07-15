#pragma once

#include <bitset>       // DEBT, make an estd one and use it

// DEBT: Not quite right namespace, 'impl'
namespace embr { namespace j1939 { namespace impl {

// this one will use a lot of RAM
template <size_t max>
struct network_address_and_name_tracker
{
    pdu<pgns::address_claimed> claims[max];
};


template <size_t max>
struct network_address_tracker
{
    // starting from 128
    std::bitset<max> claimed;
};


}}}