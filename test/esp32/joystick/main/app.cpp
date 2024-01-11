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
    if(button1.eval())  q.send_from_isr(Event{button1.state(), 0});
#if CONFIG_BUTTON2
    if(button2.eval())  q.send_from_isr(Event{button2.state(), 1});
#endif
#if CONFIG_BUTTON3
    if(button3.eval())  q.send_from_isr(Event{button3.state(), 2});
#endif
}


void App::on_notify(TWAI::event::autorx e)
{
    ESP_LOGV(TAG, "on_notify: TWAI::event::autorx");

    const transport_type::frame& frame = e.message;

    out.rdbuf()->clear();
    const auto& out_s = out.rdbuf()->str();

    embr::j1939::process_incoming(dca, transport(), frame);
    embr::j1939::process_incoming(nca, transport(), frame);

    // diagnostic_ca emits eol into 'out'
    esp_log_write(ESP_LOG_INFO, TAG, out_s.data());
}


void App::on_notify(TWAI::event::alert e)
{
    ESP_LOGV(TAG, "on_notify: TWAI::event:alert");
}

// DEBT: Move nca & associated scheduler to self-contain inside of App
void App::on_notify(changed<Service::id::substate> e, const TWAI& svc)
{
    ESP_LOGD(TAG, "on_notify: substate: %s", to_string(e.value));

    switch(e.value)
    {
        case Service::Running:
            nca.start(transport());
            break;

        default:
            break;
    }
}

// DEBT: Have to pass in TWAI& instead of const TWAI& because PropertyChanged
// for some reason wants a non-const pointer
/*
void App::on_notify(changed<Service::id::state> e, TWAI& svc)
*/

// None of this works, so going specialization route
//#define BJM_PASTER(num) j1939::pgns::basic_joystick_message_ ## num
//#define BJM_PGN    BJM_PASTER(CONFIG_J1939_BJM_NUMBER)
//#define BJM_NUMBER() CONFIG_J1939_BJM_NUMBER
//#define BJM_PGN j1939::pgns::basic_joystick_message_## CONFIG_J1939_BJM_NUMBER
//#define BJM_PGN j1939::pgns::basic_joystick_message_## BJM_NUMBER()

template <int bjm_number>
struct bjm_pgn_provider;

template <j1939::pgns pgn>
using pgn_provider = std::integral_constant<j1939::pgns, pgn>;

template <>
struct bjm_pgn_provider<1> : pgn_provider<j1939::pgns::basic_joystick_message_1> {};

template <>
struct bjm_pgn_provider<2> : pgn_provider<j1939::pgns::basic_joystick_message_2> {};

template <>
struct bjm_pgn_provider<3> : pgn_provider<j1939::pgns::basic_joystick_message_3> {};

template <>
struct bjm_pgn_provider<4> : pgn_provider<j1939::pgns::basic_joystick_message_4> {};


void App::poll()
{
    Event event;

    if(q.receive(&event, 0s))
    {
        ESP_LOGI(TAG, "poll: event=%s", embr::to_string(event.state));

        constexpr j1939::pgns bjm_pgn = bjm_pgn_provider<CONFIG_J1939_BJM_NUMBER>::value;

        j1939::pdu<bjm_pgn> pdu;

        if(nca.address().has_value())
            pdu.source_address(source_address());

        j1939::spn::discrete_parameters on_or_off = event.state == debounce::v1::States::On ?
            j1939::spn::discrete_parameters::on : 
            j1939::spn::discrete_parameters::off;

        // DEBT: Sorta abusing 'pin' here, maybe we'd prefer to call it
        // 'id' or 'user' (userdata) instead?.  We don't *have* to user
        // reference Event either...
        switch(event.pin)
        {
            case 0:
                pdu.button1_pressed(on_or_off);
                break;

            case 1:
                pdu.button2_pressed(on_or_off);
                break;

            case 2:
                pdu.button3_pressed(on_or_off);
                break;
        }

        transport_traits::send(transport(), pdu);
    }
}