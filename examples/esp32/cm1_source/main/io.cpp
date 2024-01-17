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

}