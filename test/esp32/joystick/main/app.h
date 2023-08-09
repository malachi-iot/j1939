#pragma once

#include <estd/port/freertos/queue.h>

#include <embr/property/v1/notifier.h>
#include <embr/service/v1/service.h>

#include <embr/platform/esp-idf/service/gpio.h>
#include <embr/platform/esp-idf/service/twai.h>
#include <embr/platform/esp-idf/service/gptimer.h>

#include <embr/platform/esp-idf/debounce2.h>

#include "ca.h"
#include "nca.h"

class App : transport_type
{
    static constexpr const char* TAG = "App";

    ostream_type out;
    dca_type dca;

    embr::esp_idf::debounce::v1::ultimate::Debouncer<CONFIG_GPIO_BUTTON1, true> button1;

    template <class Property>
    using changed = embr::property::v1::event::PropertyChanged<Property>;

    using Event = embr::debounce::v1::Event;

    using transport_traits = embr::j1939::transport_traits<transport_type>;

    estd::freertos::layer1::queue<Event, 10> q;

public:
    App() :
        dca(out)
    {}

    using GPIO = embr::esp_idf::service::v1::GPIO<false>;
    using TWAI = embr::esp_idf::service::v1::TWAI;
    using Timer = embr::esp_idf::service::v1::GPTimer;

    transport_type& transport() { return *this; }

    void on_notify(GPIO::event::gpio);
    void on_notify(TWAI::event::autorx);
    void on_notify(Timer::event::callback);

    void on_notify(changed<embr::Service::id::substate> e, const TWAI&);

    void poll();
};

