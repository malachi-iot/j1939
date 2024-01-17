#include <embr/platform/esp-idf/service/diagnostic.h>
#include <embr/platform/esp-idf/service/event.hpp>
#include <embr/platform/esp-idf/service/gptimer.hpp>
#include <embr/platform/esp-idf/service/twai.hpp>

#include <j1939/ca.hpp> // for 'process_incoming'

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

// NOTE: This guy is a sparse service, so technically we could use a temporary
static embr::esp_idf::service::v1::EventLoop::runtime<tier0> event_loop;

using name_type = embr::j1939::layer0::NAME<true,
    embr::j1939::industry_groups::on_highway,
    embr::j1939::vehicle_systems::non_specific,
    embr::j1939::function_fields::cab_climate_control>;

App::App() :
    dca_(out_),
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

    event_loop.start();

    event_loop.handler_register<embr::DEBOUNCE_EVENT>();

    init_gpio();
    init_timer();
    
    scheduler_.start();
}


void App::on_notify(TWAI::event::autorx e)
{
    const transport_type::frame& frame = e.message;

    out_.rdbuf()->clear();
    const auto& out_s = out_.rdbuf()->str();

    embr::j1939::process_incoming(dca_, transport(), frame);
    embr::j1939::process_incoming(nca_, transport(), frame);

    // diagnostic_ca emits eol into 'out'
    esp_log_write(ESP_LOG_INFO, TAG, out_s.data());
}

void App::on_notify(TWAI::event::alert)
{

}

void App::on_notify(changed<Service::id::substate> e, const TWAI&)
{
    switch(e.value)
    {
        case Service::Running:
            nca_.start(transport());
            break;

        default:
            break;
    }
}


// Debounce event
void App::on_notify(Event e)
{
    ESP_LOGV(TAG, "on_notify: Debounce event");
    if(e.state == embr::debounce::v1::States::Pressed)
    {
        bump();
    }
}


void App::poll()
{
    twai.poll(100 / portTICK_PERIOD_MS);
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