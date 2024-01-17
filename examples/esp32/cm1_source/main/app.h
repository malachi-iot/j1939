#pragma once

// DEBT: Clean up file naming if we can to debounce.h, document and
// ref that document here
#include <embr/platform/esp-idf/debounce2.h>

#include <embr/platform/esp-idf/service/gpio.h>
#include <embr/platform/esp-idf/service/twai.h>
#include <embr/platform/esp-idf/service/gptimer.h>

#include "ca.h"

// FIX: Put these into a Kconfig
#define CONFIG_GPIO_BUTTON1 0
#define CONFIG_ACTIVE_STATE_BUTTON1 0

namespace app {

class App
{
    static constexpr const char* TAG = "App";

    template <int gpio, bool invert>
    using db = embr::esp_idf::debounce::v1::ultimate::Debouncer<gpio, invert>;

    using Service = embr::service::v1::Service;

    db<CONFIG_GPIO_BUTTON1, CONFIG_ACTIVE_STATE_BUTTON1> button1;

    ostream_type out_;
    dca_type dca_;
    nca_type nca_;
    scheduler_type scheduler_;
    transport_type transport_;

    template <class Property>
    using changed = embr::property::v1::event::PropertyChanged<Property>;

public:
    App();

    void start();

    using TWAI = embr::esp_idf::service::v1::TWAI;
    using Timer = embr::esp_idf::service::v1::GPTimer;

    transport_type& transport() { return transport_; }

    void on_notify(TWAI::event::autorx);
    void on_notify(TWAI::event::alert);
    void on_notify(Timer::event::callback);

    void on_notify(changed<Service::id::substate> e, const TWAI&);

    void poll();
};

extern App app;

}