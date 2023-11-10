#ifdef ESP_PLATFORM

#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/twai.h>

// DEBT: Rework this to use EMBR specified values.  We don't sacrifice simplicity
// since code can otherwise stay the same

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

#endif