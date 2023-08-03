#pragma once

#include <can/platform/esp-idf/transport.hpp>

// blocking
// DEBT: I'd prefer nonblocking one right now... I think
using transport_type = embr::can::esp_idf::twai_transport<true>;

