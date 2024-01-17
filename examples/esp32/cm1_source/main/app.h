#pragma once

#include <embr/platform/esp-idf/service/gpio.h>
#include <embr/platform/esp-idf/service/twai.h>
#include <embr/platform/esp-idf/service/gptimer.h>

#include "ca.h"

namespace app {

class App
{
    static constexpr const char* TAG = "App";

    //ostream_type out_;
    //dca_type dca_;
    nca_type nca_;
    scheduler_type scheduler_;
    transport_type transport_;

    template <class Property>
    using changed = embr::property::v1::event::PropertyChanged<Property>;

public:
    App();

    void start();

    using GPIO = embr::esp_idf::service::v1::GPIO<false>;
    using TWAI = embr::esp_idf::service::v1::TWAI;
    using Timer = embr::esp_idf::service::v1::GPTimer;

    transport_type& transport() { return transport_; }

    void on_notify(GPIO::event::gpio);
    void on_notify(TWAI::event::autorx);
    void on_notify(TWAI::event::alert);
    void on_notify(Timer::event::callback);

    void on_notify(changed<embr::Service::id::substate> e, const TWAI&);

    void poll();
};

}