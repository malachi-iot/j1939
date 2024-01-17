#include <esp_log.h>

// DEBT: estd::variadic needs std::forward and friends.  Put this out
// into estd proper
#include <estd/utility.h>

#include <embr/platform/esp-idf/board.h>

using board_traits = embr::esp_idf::board_traits;

extern "C" void app_main(void)
{
    static const char* TAG = "app_main";

    ESP_LOGD(TAG, "start: board=%s %s",
        board_traits::vendor,
        board_traits::name);
}