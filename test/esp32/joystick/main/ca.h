#pragma once

#include <estd/sstream.h>

#include <can/platform/esp-idf/transport.hpp>

#include <j1939/cas/diagnostic.hpp>

// blocking
// DEBT: I'd prefer nonblocking one right now... I think
using transport_type = embr::can::esp_idf::twai_transport<true>;

using ostream_type = estd::layer1::ostringstream<256>;

// DEBT: impl vs non impl here
using dca_type = embr::j1939::diagnostic_ca<transport_type, ostream_type>;

