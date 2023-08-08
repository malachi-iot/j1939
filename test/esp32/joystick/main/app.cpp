#include <esp_log.h>

#include <embr/platform/esp-idf/debounce2.h>

#include <j1939/ca.hpp>

#include "app.h"
#include "nca.h"

using namespace estd::chrono_literals;

using namespace embr;

void App::on_notify(GPIO::event::gpio gpio)
{

}


void App::on_notify(Timer::event::callback)
{
    if(button1.eval())
    {
        q.send_from_isr(Event{button1.state(), 0});
    }
}


void App::on_notify(TWAI::event::rx)
{
    ESP_LOGV(TAG, "on_notify: TWAI::event::rx");

    transport_type::frame frame;

    // DEBT: Make a while loop, and probably go into polling mode -- OR --
    // use autorx
    bool result = transport_type::receive(&frame);

    if(!result)
    {
        ESP_LOGW(TAG, "receive timed out");
        return;
    }

    out.rdbuf()->clear();
    const auto& out_s = out.rdbuf()->str();

    embr::j1939::process_incoming(dca, transport(), frame);
    embr::j1939::process_incoming(nca, transport(), frame);

    // diagnostic_ca emits eol into 'out'
    esp_log_write(ESP_LOG_INFO, TAG, out_s.data());
}


// DEBT: Move nca & associated scheduler to self-contain inside of App
void App::on_notify(changed<Service::id::substate> e, const TWAI& svc)
{
    ESP_LOGD(TAG, "on_notify: substate: %s", to_string(e.value));

    switch(e.value)
    {
        case Service::Running:
            nca_init(transport());
            break;

        default:
            break;
    }
}

// DEBT: This is a wrapper because major state events do NOT emit substate
// events.  There was a method to that madness, but now it feels like an oversight.
// Document why we WANT that behavior, or, revise it
// DEBT: Have to pass in TWAI& instead of const TWAI& because PropertyChanged
// for some reason wants a non-const pointer
void App::on_notify(changed<Service::id::state> e, TWAI& svc)
{
    switch(e.value)
    {
        case Service::Started:
            on_notify(changed<Service::id::substate>{&svc, svc.substate()}, svc);
            break;

        default: break;
    }
}



void App::poll()
{
    Event event;

    if(q.receive(&event, 0s))
    {
        ESP_LOGI(TAG, "poll: event=%s", embr::to_string(event.state));

        j1939::pdu<j1939::pgns::basic_joystick_message_1> pdu;

        if(nca.address().has_value())
            pdu.source_address(nca.address().value());

        pdu.button1_pressed(event.state == debounce::v1::States::On ?
            j1939::spn::discrete_parameters::on : 
            j1939::spn::discrete_parameters::off);

        transport_traits::send(transport(), pdu);
    }
}