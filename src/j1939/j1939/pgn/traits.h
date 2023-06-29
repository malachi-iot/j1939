#pragma once

#include "fwd.h"
#include "enum.h"

namespace embr { namespace j1939 { namespace pgn {

#if __cpp_concepts
#endif


template <>
struct traits<pgns::oel>
{
    static constexpr const char* name()
    {
        return "Operator External Light Controls";
    }

    static constexpr const char* abbrev() { return "OEL"; }
};


template <>
struct traits<pgns::fan_drive_1>
{
    static constexpr const char* name()
    {
        return "Fan Drive 1";
    }

    static constexpr const char* abbrev() { return "FD1"; }
};


template <>
struct traits<pgns::cab_message1>
{
    static constexpr const char* name()
    {
        return "Cab Message 1";
    }

    static constexpr const char* description()
    {
        return "Message containing parameters originating from the vehicle cab.";
    }

    static constexpr const char* abbrev() { return "CM1"; }
};


template <>
struct traits<pgns::cab_message_3>
{
    static constexpr const char* name()
    {
        return "Cab Message 3";
    }

    static constexpr const char* description()
    {
        return "Provides information from Cab mounted operator inputs.";
    }

    static constexpr const char* abbrev() { return "CM3"; }
};


}}}