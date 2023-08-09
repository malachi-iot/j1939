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

void twai_init()
{
    constexpr twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        (gpio_num_t)CONFIG_GPIO_TWAI_TX,
        (gpio_num_t)CONFIG_GPIO_TWAI_RX,
        TWAI_MODE_NORMAL);
    constexpr twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();

    app_domain::twai.start(&g_config, &t_config);
    app_domain::twai.autorx(true);
}

#define GPIO_INPUT_IO_0     CONFIG_GPIO_BUTTON1
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)

void gpio_init()
{
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    // DEBT: Service is specifically ISR-based GPIO input, reflect that
    // in naming
    // DEBT: We prefer debouncer in which case gpio service is not wanted at all,
    // but the io config still is
    app_domain::gpio.start(&io_conf);
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

    twai_init();
    gpio_init();
    timer_init();

    ESP_LOGI(TAG, "start: sizeof(App)=%u", sizeof(App));

    for(;;)
    {
        // DEBT: Find a way to satisfy watchdog/yield requirements so that we
        // can have a 0 tick timeout

        app_domain::app.poll();
        app_domain::twai.poll(100 / portTICK_PERIOD_MS);
    }
}
