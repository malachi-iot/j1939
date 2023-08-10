#pragma once


#include <estd/ostream.h>

#include <j1939/pdu.h>
#include <j1939/ca.hpp>
#include <j1939/data_field/lighting_command.hpp>
#include <j1939/data_field/lighting_data.hpp>

#include "conf.h"

struct ArduinoLightingCommandCa : embr::j1939::impl::controller_application_base
{
};