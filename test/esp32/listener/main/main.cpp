#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/twai.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <estd/string.h>
#include <estd/sstream.h>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/ca.hpp>

#include "nca.h"
#include "streambuf.h"

using ostream_type = estd::experimental::ostringstream<256>;

// DEBT: impl vs non impl here
using dca_type = embr::j1939::diagnostic_ca<transport_type, ostream_type>;

static estd::detail::basic_ostream<esp_idf::impl::log_ostreambuf<estd::char_traits<char> >> clog;

void twai_init()
{
    // Oddly, at the moment, if in TWAI_MODE_LISTENER_ONLY we receive from M4:
    // - 6x of OEL or AC pdu
    // - infinite CM1 pdu (observe even when re-flashing ESP32)
    // Feels like a wiring issue.  Not sure.

    static twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        (gpio_num_t)CONFIG_GPIO_TWAI_TX,
        (gpio_num_t)CONFIG_GPIO_TWAI_RX,
        TWAI_MODE_NORMAL);  // DEBT: Probably do listener only for this particular scenario
        // DEBT: If we go into listener only mode, I think M4 goes crazy with retries.  I
        // really didn't expect that, yet somehow it feels familiar once I noticed it
        //TWAI_MODE_LISTEN_ONLY);
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();

    //esp_err_t ret;

    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());

    // We want all alerts since this is a diagnostic app
    ESP_ERROR_CHECK(twai_reconfigure_alerts(0xFFFF, nullptr));
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

    // DEBT: 'household' project twai service would come in handy here
    twai_init();

    // DEBT: Just to satisfy process_incoming signature & friends, actually an empty struct
    transport_type t;

    // NOTE: Works great, but this is app is not a "listener" role
    //nca_init(t);

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
