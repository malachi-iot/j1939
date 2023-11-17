#pragma once

// DEBT: chrono-chooser in estd does an if/else for FreeRTOS vs Arduino
// when it should include both
#include <estd/port/arduino/chrono.h>
#include <estd/ostream.h>

#include <embr/scheduler.hpp>

#include <j1939/pdu.h>
#include <j1939/ca.hpp>
#include <j1939/data_field/cm1.hpp>

#include "conf.h"
#include "transport.h"

using scheduler_type = embr::internal::layer1::Scheduler<
    5,
    embr::internal::scheduler::impl::Function<estd::chrono::arduino_clock::time_point> >;

//using ArduinoLightingCommandCa = embr::j1939::impl::lighting_command_ca<transport, scheduler_type>;
