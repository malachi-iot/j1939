#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/twai.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <estd/string.h>
#include <estd/sstream.h>

#include <j1939/ca.hpp>

#include <embr/platform/esp-idf/service/diagnostic.h>
#include <embr/platform/esp-idf/service/gpio.hpp>
#include <embr/platform/esp-idf/service/gptimer.hpp>
#include <embr/platform/esp-idf/service/twai.hpp>

#include "app.h"

#include "nca.h"
#include "streambuf.h"

static esp_idf::log_ostream clog;   // Coming along well, almost ready

extern void twai_init();

namespace app_domain {

App app;

using Diagnostic = embr::esp_idf::service::v1::Diagnostic;

typedef estd::integral_constant<App*, &app> app_singleton;
typedef embr::layer0::subject<Diagnostic, app_singleton> filter_observer;

App::GPIO::runtime<filter_observer> gpio;
App::TWAI::runtime<filter_observer> twai;
App::Timer::runtime<filter_observer> timer;

}

void twai_init2()
{
    static twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        (gpio_num_t)CONFIG_GPIO_TWAI_TX,
        (gpio_num_t)CONFIG_GPIO_TWAI_RX,
        TWAI_MODE_NORMAL);
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();

    app_domain::twai.start(&g_config, &t_config, &f_config);
    app_domain::twai.autorx(false);
}

void gpio_init()
{

}


void timer_init()
{
    gptimer_config_t config = 
    {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
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

    app_domain::timer.start(&config, &alarm_config);
}

extern "C" void app_main(void)
{
    static const char* TAG = "app_main";

    twai_init2();
    gpio_init();
    timer_init();

    // DEBT: Just to satisfy process_incoming signature & friends, actually an empty struct
    transport_type& t = app_domain::app.transport();

    nca_init(t);

    uint32_t prev_alerts = 0;

    ostream_type out;
    dca_type dca(out);

    for(;;)
    {
        // TODO: This does work, but it eats the alert so don't activate it yet
        //app_domain::twai.poll(0);

        out.rdbuf()->clear();
        const auto& out_s = out.rdbuf()->str();

        uint32_t alerts = 0;

        twai_read_alerts(&alerts, 0);

        if(alerts != prev_alerts)
        {
            ESP_LOGD(TAG, "alerts: %" PRIx32, alerts);
            prev_alerts = alerts;
        }

        if(alerts & TWAI_ALERT_RX_DATA)
        {
            transport_type::frame frame;

            // NOTE: Occasionally we are getting RX overflow.  That's kind of odd.
            // Perhaps bus error is causing a lot of quick retries?
            bool result = transport_type::receive(&frame);

            if(!result)
            {
                ESP_LOGW(TAG, "receive timed out");
                continue;
            }

            // Prefer not aggregating them since as a diagnostic app we prefer
            // any glitches to be easier to diagnose
            embr::j1939::process_incoming(dca, t, frame);
            embr::j1939::process_incoming(nca, t, frame);

            // TODO: Could be interesting to make an 'out' which uses esp-idf's low level
            // log output facility and/or low level serial output

            // diagnostic_ca emits eol into 'out'
            esp_log_write(ESP_LOG_INFO, TAG, out_s.data());

            // This just clears format/error flags, we need an out.rdbuf()->clear();
            //out.clear();
        }
        else
            // Just in case of problems, wait a little to avoid overly spamming
            vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
