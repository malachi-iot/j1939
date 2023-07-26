#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/twai.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <estd/string.h>
#include <estd/sstream.h>

#include <embr/scheduler.hpp>
#include <embr/platform/freertos/scheduler.hpp>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/cas/network.hpp>
#include <j1939/cas/internal/prng_address_manager.h>
#include <j1939/NAME/name.h>

#include <can/platform/esp-idf/transport.hpp>
#include <j1939/ca.hpp>

#include "streambuf.h"

// blocking
// DEBT: I'd prefer nonblocking one right now... I think
using transport_type = embr::can::esp_idf::twai_transport<true>;

using ostream_type = estd::experimental::ostringstream<256>;

// DEBT: freertos Scheduler works, but its innards are sloppy
using scheduler_type = embr::scheduler::freertos::Scheduler<10>;

// DEBT: impl vs non impl here
// DEBT: default address manager should be something better than 'void'
using dca_type = embr::j1939::diagnostic_ca<transport_type, ostream_type>;
using nca_type = embr::j1939::impl::network_ca<transport_type, scheduler_type,
    embr::j1939::internal::prng_address_manager>;

static estd::detail::basic_ostream<esp_idf::impl::log_ostreambuf<estd::char_traits<char> >> clog;

void twai_init()
{
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

    // Oddly, at the moment, we receive from M4:
    // - 6x of OEL or AC pdu
    // - infinite CM1 pdu (observe even when re-flashing ESP32)
    // Feels like a wiring issue.  Not sure.

    twai_reconfigure_alerts(0xFFFF, nullptr);

    static scheduler_type scheduler;
    
    using proto_name = embr::j1939::layer0::NAME<true,
        embr::j1939::industry_groups::process_control,
        embr::j1939::vehicle_systems::ig5_not_available, // DEBT: Change to a better IG/Veh Sys,
        embr::j1939::function_fields::ig5_not_available>;

    // DEBT: Can't inint this name with proto_name::sparse.
    // Not important for this test app, but important overall
    //static embr::j1939::layer1::NAME name;

    static nca_type nca(proto_name::sparse(1, 0, 0), scheduler);

    // Kicks off its own FreeRTOS task.
    scheduler.start();

    // DEBT: Just to satisfy process_incoming signature & friends, actually an empty struct
    transport_type t;

    // NOTE: Works great, but this is not a "listener" role
    //nca.start(t);

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
