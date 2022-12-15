#pragma once

namespace embr { namespace can {

// NOTE: Consider melding all this with service code
enum class bus_state
{
    uninitialized,
    offline,
    online,
    bus_off,
    recovering,
    unknown             // other unspecified error
};

enum class status
{
    ok,
    timeout,
    fail,
    bad_driver,
    not_supported
};

}}