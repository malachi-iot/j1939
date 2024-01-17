#include "app.h"


void init_gpio()
{

}

void init_twai()
{

}


namespace app {

// Remember, this is ISR context
void App::on_notify(Timer::event::callback)
{
    // TODO: Emit event or other isr-safe signal (feeling event
    // for thiso ne)
    if(button1.eval())  {}
}

}