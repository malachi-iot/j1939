#pragma once

#include "main.h"

#include <embr/scheduler.hpp>
#include <embr/platform/freertos/scheduler.hpp>

#include <j1939/cas/network.hpp>
#include <j1939/cas/internal/prng_address_manager.h>
#include <j1939/NAME/name.h>

// DEBT: freertos Scheduler works, but its innards are sloppy
using scheduler_type = embr::scheduler::freertos::Scheduler<10>;

// DEBT: default address manager should be something better than 'void'
using nca_type = embr::j1939::impl::network_ca<transport_type, scheduler_type,
    embr::j1939::internal::prng_address_manager>;

void nca_init(transport_type& t);

extern nca_type nca;