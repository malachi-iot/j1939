#include <esp_log.h>

#include "app.h"

void App::on_notify(GPIO::event::gpio gpio)
{

}


void App::on_notify(Timer::event::callback gpio)
{

}


void App::on_notify(TWAI::event::rx)
{
    ESP_LOGI(TAG, "on_notify: TWAI::event::rx");
}
