#include <embr/platform/esp-idf/service/diagnostic.h>
#include <embr/platform/esp-idf/service/gptimer.hpp>
#include <embr/platform/esp-idf/service/twai.hpp>

#include "app.h"
#include "io.h"

// DEBT: Consider a j1939-wide Kconfig for these.  Note that
// it's strongly suggested NAME and SA are 1:1 and each CA in
// one firmware is supposed to have its own SA
#define CONFIG_J1939_VEHICLE_SYSTEM_INSTANCE 0
#define CONFIG_J1939_FUNCTION_INSTANCE 0
#define CONFIG_J1939_ECU_INSTANCE 0


static void init_timer();

namespace app {

App app;

using Diagnostic = embr::esp_idf::service::v1::Diagnostic;

using Singleton = estd::integral_constant<App*, &app>;
using tier0 = embr::layer0::subject<Diagnostic, Singleton>;

static App::TWAI::runtime<tier0> twai;
static App::Timer::runtime<tier0> timer;

using name_type = embr::j1939::layer0::NAME<true,
    embr::j1939::industry_groups::on_highway,
    embr::j1939::vehicle_systems::non_specific,
    embr::j1939::function_fields::cab_climate_control>;

App::App() :
    nca_(name_type::sparse(
        CONFIG_J1939_VEHICLE_SYSTEM_INSTANCE,
        CONFIG_J1939_FUNCTION_INSTANCE,
        CONFIG_J1939_ECU_INSTANCE), scheduler_)
{

}


void App::start()
{
    twai.start();
    twai.autorx(true);  // DEBT: Shouldn't he precede start? if no, comment why

    init_gpio();
    init_timer();
    
    scheduler_.start();
}


void App::on_notify(TWAI::event::autorx)
{

}

void App::on_notify(TWAI::event::alert)
{

}

}


static void init_timer()
{
    gptimer_config_t config = 
    {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
        .intr_priority = 0,                 // unsure if esp-idf < 5.1.2 has this
        .flags
        {
            .intr_shared = true
        }
    };

    gptimer_alarm_config_t alarm_config =
    {
        .alarm_count = 10000, // period = 10ms @resolution 1MHz
        .reload_count = 0, // counter will reload with 0 on alarm event
        .flags
        {
            .auto_reload_on_alarm = true, // enable auto-reload
        }
    };

    app::timer.start(&config, &alarm_config);
}