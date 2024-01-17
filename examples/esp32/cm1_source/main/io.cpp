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
void App::on_notify(Timer::event::callback)
{
    // TODO: Emit event or other isr-safe signal (feeling event
    // for thiso ne)
    if(button1.eval())  {}
}

}