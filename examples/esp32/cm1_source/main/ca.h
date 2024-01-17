#pragma once

#include <estd/sstream.h>

#include <embr/scheduler.hpp>
#include <embr/platform/freertos/scheduler.hpp>

#include <can/platform/esp-idf/transport.hpp>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/cas/network.hpp>
#include <j1939/cas/internal/prng_address_manager.h>
#include <j1939/NAME/name.h>

namespace app {

//using namespace embr::j1939;

// blocking
// DEBT: We prefer embr::esp_idf::can 
using transport_type = embr::can::esp_idf::twai_transport<true>;

// DEBT: use new embr/j1939 esp-idf cout logger here
using ostream_type = estd::layer1::ostringstream<256>;

// DEBT: impl vs non impl here
using dca_type = embr::j1939::diagnostic_ca<transport_type, ostream_type>;

// DEBT: freertos Scheduler works, but its innards are sloppy
// DEBT: there's a v2-ish flavor on the way, try that one out
using scheduler_type = embr::scheduler::freertos::Scheduler<5>;

// DEBT: default address manager should be something better than 'void'
using nca_type = embr::j1939::impl::network_ca<transport_type, scheduler_type,
    embr::j1939::internal::prng_address_manager>;

}