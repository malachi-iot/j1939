#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/twai.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <estd/string.h>
#include <estd/sstream.h>

#include <j1939/cas/diagnostic.hpp>
#include <can/platform/esp-idf/transport.hpp>
#include <j1939/ca.hpp>

#include "streambuf.h"

// blocking
// DEBT: I'd prefer nonblocking one right now... I think
using transport_type = embr::can::esp_idf::twai_transport<true>;

using ostream_type = estd::experimental::ostringstream<256>;
using dca_type = embr::j1939::diagnostic_ca<transport_type, ostream_type>;

static ostream_type out;
static estd::detail::basic_ostream<esp_idf::impl::log_ostreambuf<estd::char_traits<char> >> clog;
dca_type dca(out);

void twai_init()
{
    static twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        (gpio_num_t)CONFIG_GPIO_TWAI_TX,
        (gpio_num_t)CONFIG_GPIO_TWAI_RX,
        TWAI_MODE_NORMAL);  // DEBT: Probably do listener only for this particular scenario
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();

    //esp_err_t ret;

    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());
}

extern "C" void app_main(void)
{
    static const char* TAG = "app_main";

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    twai_init();

    ESP_LOGI(TAG, "Startup");

    const auto& out_s = out.rdbuf()->str();

    for(;;)
    {
        transport_type::frame frame;

        transport_type::receive(&frame);

        transport_type t;   // DEBT: Just to satisfy process_incoming signature, actually an empty struct

        embr::j1939::process_incoming(dca, t, frame);

        // TODO: Output what 'out' gathered
        // TODO: Could be interesting to make an 'out' which uses esp-idf's low level
        // log output facility and/or low level serial output

        esp_log_write(ESP_LOG_INFO, TAG, out_s.data());

        //vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
