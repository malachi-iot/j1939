#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/twai.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <estd/string.h>
#include <estd/sstream.h>

#include <j1939/ca.hpp>
#include <j1939/cas/internal/time_date.h>

#include <embr/platform/esp-idf/board.h>

#include <embr/platform/esp-idf/service/diagnostic.h>
#include <embr/platform/esp-idf/service/gpio.hpp>
#include <embr/platform/esp-idf/service/gptimer.hpp>
#include <embr/platform/esp-idf/service/twai.hpp>

#include "app.h"

#include "nca.h"
#include "streambuf.h"

static esp_idf::log_ostream clog;   // Coming along well, almost ready

extern void twai_init();

using board_traits = embr::esp_idf::board_traits;

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
    app_domain::twai.start();
    app_domain::twai.autorx(true);
}

namespace embr_R = embr::esp_idf::R;

using leds = board_traits::io::where<
        embr::R::traits_selector<
            embr_R::led> >;

// DEBT: Find a way to hang/stack status_leds off above leds finder
using status_leds = board_traits::io::where<
        embr::R::traits_selector<
            embr_R::led,
            embr_R::trait::status> >;

template <class Selected, class Return>
constexpr Return single_or_default(Return def = {})
{
    if constexpr(Selected::size() == 1)
        return Selected::single::pin;
    else
        return def;
}

template <class GPIO>
void led_off(GPIO)
{
    static constexpr const char* TAG = "led_off";

    constexpr embr::esp_idf::gpio led(GPIO::pin);

    led.set_direction(GPIO_MODE_OUTPUT);
    led.level(0);

    ESP_LOGD(TAG, "pin = %u", (gpio_num_t)led);
}


template <class... Selected>
void leds_off2(Selected ... selected)
{
    (led_off(selected), ...);
}

template <class... Selected>
void leds_off(estd::variadic::types<Selected...>)
{
    leds_off2(Selected{}...);
}

constexpr gpio_num_t status_led_pin()
{
    return single_or_default<status_leds>((gpio_num_t)-1);
}

// DEBT: Can't do this because no constexpr destructor
//constexpr estd::optional<int> tester() { return 0; }

constexpr embr::esp_idf::gpio led(status_led_pin());

#define GPIO_INPUT_IO_0     CONFIG_GPIO_BUTTON1
#ifndef CONFIG_GPIO_BUTTON2
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)
#else
#define GPIO_INPUT_IO_1     CONFIG_GPIO_BUTTON2
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#endif

void gpio_init()
{
    // Succeeds in turning off LEDs, but main ESP32 devkit has same GPIO for an LED as
    // default CAN TX pin, so not doing this just yet
    //leds_off(leds{});

    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    // DEBT: Service is specifically ISR-based GPIO input, reflect that
    // in naming.
    // DEBT: We'd prefer to use esp-idf's driver-based gpio here rather than straight ISR
    // DEBT: We prefer debouncer in which case gpio service is not wanted at all,
    // but the io config still is
    // DEBT: A bit magical to know that ESP_INTR_FLAG_LEVEL1 the default used by both our GPIO
    // handler as well as TWAI.  That's why we specify the not-ideal level 2 here (higher priority
    // interrupt on a human oriented ISR) - note we only hit this on ESP32S3, but leaving the
    // change in for others.  I keep thinking timer itself is also involved in this, but can't prove it
    app_domain::gpio.start(&io_conf, ESP_INTR_FLAG_LEVEL2);

    if constexpr(status_leds::size())
    {
        // Sometimes LED is on same pin as button, in which case don't use it
        if constexpr (led != GPIO_INPUT_IO_0)
            led.set_direction(GPIO_MODE_OUTPUT);
    }
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


void time_date_emitter_init()
{
    // DEBT: Doesn't belong in joystick per se but just fleshing it out for now
    // DEBT: Not obvious that source_address is a reference and therefore auto updates
    // into the functor
    embr::j1939::internal::emit_time_date_functor<transport_type>
        emit_td{app_domain::app.transport(), app_domain::app.source_address() };

    using function_type = scheduler_type::function_type;

    // DEBT: Pretty sure we (theoretically) don't have to do a move here,
    // although make_model does demand it.  That said, it's convenient to do
    // so so that we can put emit_td on the stack and model itself bears
    // full allocation responsibility
    static auto model = function_type::make_model(std::move(emit_td));

    // TODO: Needs that magical estd::detail::function / model glue here
    app_domain::app.scheduler().schedule_now(&model);
}

extern "C" void app_main(void)
{
    static const char* TAG = "app_main";

    ESP_LOGD(TAG, "start: vendor=%s board=%s",
        board_traits::vendor,
        board_traits::name);

    twai_init();
    gpio_init();
    timer_init();
    time_date_emitter_init();

    ESP_LOGI(TAG, "start: sizeof(App)=%u", sizeof(App));

    unsigned counter = 0;
    bool led_on = false;

    // NOTE: If CAN bus isn't physically online, the transmits will eventually hang for ~3 minutes
    // Oddly I didn't see any alert events before then.

    for(;;)
    {
        
        // DEBT: Find a way to satisfy watchdog/yield requirements so that we
        // can have a 0 tick timeout

        app_domain::app.poll();     // Notice any ISR-sourced button presses
        app_domain::twai.poll(100 / portTICK_PERIOD_MS);

        ++counter;

        if(counter % 50 == 0) ESP_LOGV(TAG, "counter: %u", counter);

        if((counter % 10 == 0) && led != -1)
            led.level(led_on = !led_on);
    }
}
