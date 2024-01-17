#include "app.h"
#include "io.h"

// DEBT: Consider a j1939-wide Kconfig for these
#define CONFIG_J1939_VEHICLE_SYSTEM_INSTANCE 0
#define CONFIG_J1939_FUNCTION_INSTANCE 0
#define CONFIG_J1939_ECU_INSTANCE 0

namespace app {

using name_type = embr::j1939::layer0::NAME<true,
    embr::j1939::industry_groups::on_highway,
    embr::j1939::vehicle_systems::non_specific,
    embr::j1939::function_fields::cab_climate_control>;

App app;

App::App() :
    nca_(name_type::sparse(
        CONFIG_J1939_VEHICLE_SYSTEM_INSTANCE,
        CONFIG_J1939_FUNCTION_INSTANCE,
        CONFIG_J1939_ECU_INSTANCE), scheduler_)
{

}


void App::start()
{
    init_twai();
    init_gpio();
    
    scheduler_.start();
}

}