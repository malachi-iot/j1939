#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/twai.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <estd/string.h>
#include <estd/sstream.h>

#include <j1939/ca.hpp>

#include <embr/platform/esp-idf/service/gpio.hpp>

#include "app.h"

#include "nca.h"
#include "streambuf.h"

static esp_idf::log_ostream clog;   // Coming along well, almost ready

extern void twai_init();


App app;

extern "C" void app_main(void)
{
    static const char* TAG = "app_main";

    // DEBT: 'household' project twai service would come in handy here
    twai_init();

    // DEBT: Just to satisfy process_incoming signature & friends, actually an empty struct
    transport_type& t = app.transport();

    nca_init(t);

    uint32_t prev_alerts = 0;

    for(;;)
    {
        // DEBT: Temporarily placing 'out' here since we don't have a way to
        // clear it out just yet
        ostream_type out;
        const auto& out_s = out.rdbuf()->str();
        dca_type dca(out);
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
