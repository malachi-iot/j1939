#include <array>

#include <esp_event.h>

#include "app.h"


void init_gpio()
{

}

void init_twai()
{

}


namespace app {

#ifndef CONFIG_ESP_EVENT_POST_FROM_ISR
#error "ESP Event from ISR feature is required"
#endif

// Remember, this is ISR context
// DEBT: Put this into IRAM? If not, document why not
void App::on_notify(Timer::event::callback)
{
    if(button1.eval())  // state change?
    {
        Event e(button1.state(), button1.pin);
        esp_event_isr_post(embr::DEBOUNCE_EVENT, 0, &e, sizeof(e), nullptr);
    }
}


static constexpr std::array<short, 4> speeds { 0, 33, 66, 100 };

using namespace embr;

void App::bump()
{
    using transport_traits = j1939::transport_traits<transport_type>;
    using address_traits = j1939::spn::internal::address_type_traits_base;

    if(++index_ == speeds.size())   index_ = 0;

    units::percent<short> speed(speeds[index_]);

    j1939::pdu<j1939::pgns::cab_message1> pdu;

    pdu.destination_address(address_traits::global);

    if(nca_.address().has_value())  pdu.source_address(*nca_.address());

    pdu.requested_percent_fan_speed(speed);

    ESP_LOGD(TAG, "bump: speed=%d%% (%u)", speed.count(), pdu.requested_percent_fan_speed().count());

    transport_traits::send(transport(), pdu);
}

}