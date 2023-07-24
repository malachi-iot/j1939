#include "esp_log.h"
#include "nvs_flash.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <estd/string.h>
#include <estd/sstream.h>

#include <j1939/cas/diagnostic.hpp>
#include <can/platform/esp-idf/transport.hpp>
#include <j1939/ca.hpp>

// blocking
// DEBT: I'd prefer nonblocking one right now... I think
using transport_type = embr::can::esp_idf::twai_transport<true>;

using ostream_type = estd::experimental::ostringstream<256>;
using dca_type = embr::j1939::diagnostic_ca<transport_type, ostream_type>;

static ostream_type out;
dca_type dca(out);

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

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}