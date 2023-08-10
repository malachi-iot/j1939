#pragma once


#include <estd/ostream.h>

#include <embr/scheduler.hpp>

#include <j1939/pdu.h>
#include <j1939/ca.hpp>
#include <j1939/cas/lighting_command.hpp>
#include <j1939/data_field/lighting_command.hpp>
#include <j1939/data_field/lighting_data.hpp>

#include "conf.h"
#include "transport.h"

using scheduler_type = embr::internal::layer1::Scheduler<
    5,
    embr::internal::scheduler::impl::Function<estd::chrono::arduino_clock::time_point> >;

using ArduinoLightingCommandCa = embr::j1939::impl::lighting_command_ca<transport, scheduler_type>;
