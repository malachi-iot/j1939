#pragma once

#include <estd/port/freertos/queue.h>

#include <embr/platform/esp-idf/service/gpio.h>
#include <embr/platform/esp-idf/service/twai.h>

#include "ca.h"

class App : transport_type
{
public:
    using GPIO = embr::esp_idf::service::v1::GPIO<false>;
    using TWAI = embr::esp_idf::service::v1::TWAI;

    transport_type& transport() { return *this; }

    // TODO: For gpio notifications
    void on_notify(GPIO::event::gpio);
};

