#pragma once

#include "main.h"

#include <embr/scheduler.hpp>
#include <embr/platform/freertos/scheduler.hpp>

#include <j1939/cas/network.hpp>
#include <j1939/cas/internal/prng_address_manager.h>
#include <j1939/NAME/name.h>

#include "ca.h"

// DEBT: freertos Scheduler works, but its innards are sloppy
// DEBT: there's a v2-ish flavor on the way, try that one out
using scheduler_type = embr::scheduler::freertos::Scheduler<10>;

// DEBT: Easily collided and confusing type name
using proto_name = embr::j1939::layer0::NAME<true,
    embr::j1939::industry_groups::construction,
    embr::j1939::vehicle_systems::non_specific,
    embr::j1939::function_fields::joystick_control>;

// DEBT: default address manager should be something better than 'void'
using nca_type = embr::j1939::impl::network_ca<transport_type, scheduler_type,
    embr::j1939::internal::prng_address_manager>;
