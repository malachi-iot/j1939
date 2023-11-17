#pragma once

// DEBT: chrono-chooser in estd does an if/else for FreeRTOS vs Arduino
// when it should include both
#include <estd/port/arduino/chrono.h>
#include <estd/ostream.h>

#include <embr/scheduler.hpp>

#include <j1939/pdu.h>
#include <j1939/ca.hpp>
#include <j1939/NAME/name.h>
#include <j1939/cas/network.hpp>
#include <j1939/cas/internal/prng_address_manager.h>
#include <j1939/data_field/cm1.hpp>

#include "conf.h"
#include "transport.h"

using scheduler_type = embr::internal::layer1::Scheduler<
    5,
    embr::internal::scheduler::impl::Function<estd::chrono::arduino_clock::time_point> >;

using nca_type = embr::j1939::impl::network_ca<transport,
    scheduler_type,
    embr::j1939::internal::prng_address_manager>;


// DEBT: Could be better, but we're not too far off
using proto_name = embr::j1939::layer0::NAME<true,
    embr::j1939::industry_groups::on_highway,
    embr::j1939::vehicle_systems::trailer,
    embr::j1939::function_fields::body_to_vehicle_interface_control>;
