#pragma once

#include <estd/string.h>

#include "../data_field/network.hpp"
#include "../data_field/transport_protocol.hpp"

namespace embr { namespace j1939 {

// NOTE: Need to keep this TString and not a specific flavor so that storage
// allocation of said string below behaves right
// DEBT: This class might be more useful as a static traits-style scenario since it's
// just barely a state machine (at the moment)
template <class TString>
struct string_gatherer
{
    typedef TString string_type;
    using value_type = typename string_type::value_type;
    string_type s;

    // DEBT: Don't always presume delimiter is '*'
    // DEBT: Very likely this is more useful if we pass in expected string size also
    template <class TContainer>
    void process_incoming(const data_field<pgns::tp_dt, TContainer>& d, unsigned offset = 0)
    {
        // DEBT: That old familiar ostream/string typecast
        const value_type* data = (const value_type*)d.data();
        unsigned max_sz = 7;    // tp_dt max payoad is 7

        // DEBT: Heed sequence number
        while(max_sz-- != 0)
        {
            if(*data == '*')
            {
                return;
            }

            s += *data++;
        }
    }
};

}}