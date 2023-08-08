#pragma once

#include <estd/port/freertos/queue.h>

#include <embr/platform/esp-idf/service/gpio.h>
#include <embr/platform/esp-idf/service/twai.h>
#include <embr/platform/esp-idf/service/gptimer.h>

#include <embr/platform/esp-idf/debounce2.h>

#include "ca.h"

class App : transport_type
{
    static constexpr const char* TAG = "App";
    
public:
    using GPIO = embr::esp_idf::service::v1::GPIO<false>;
    using TWAI = embr::esp_idf::service::v1::TWAI;
    using Timer = embr::esp_idf::service::v1::GPTimer;

    transport_type& transport() { return *this; }

    void on_notify(GPIO::event::gpio);
    void on_notify(TWAI::event::rx);
    void on_notify(Timer::event::callback);
};

