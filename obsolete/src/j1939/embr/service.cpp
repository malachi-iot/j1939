// Initial guidance from:
// https://github.com/espressif/esp-idf/blob/v4.4.1/examples/protocols/sntp/main/sntp_example_main.c
#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_sntp.h"
#endif

#include "services.h"

using namespace embr;

embr::bits::internal::word<32> service_container::changed_flag{0};
unsigned service_container::id_counter = 0;

namespace services {

service_manager manager;

namespace id {

const char events[] = "events";

}

}

namespace embr {

void service_manager::state_changed(service_container& state_changed_service, int substate)
{
    // TODO: Optimize so that substates don't generate a ton of these calls,
    // perhaps service_container can indicate a bitmask of who it wants
    // status from both a general and substate sense
    for(service_container* i = head; i != nullptr; i = i->next_)
    {
        uint32_t only_i = 1 << i->id_;

        if(!i->is_interested(i->id_)) continue;

        // If only one service changed, and it's us, then don't
        // notify ourselves
        // DEBT: Something odd here, changed_flag == only_i is not filtering out
        // ourselves as expected.  Not a FIX because state_changed_service fills in that
        // gap and *might* be preferable
        if(service_container::changed_flag == only_i || i == &state_changed_service) continue;

        i->state_changed(state_changed_service, substate);
    }

    service_container::changed_flag = 0;
}


void service_manager::add(service_container& v)
{
    v.next_ = head;
    head = &v;
}


service_container* service_manager::get(unsigned id)
{
    for(service_container* i = head; i != nullptr; i = i->next_)
    {
        if(id == i->id_) return i;
    }

    return nullptr;
}


service_container* service_manager::get(const char* name)
{
    for(service_container* i = head; i != nullptr; i = i->next_)
    {
        if(name == i->name_) return i;
    }

    return nullptr;
}


}

const char* description(service_container_base::states s)
{
    typedef service_container_base::states states;

    switch(s)
    {
        case states::started: return "started";
        case states::stopped: return "stopped";
        case states::dependency: return "dependency";
        case states::error: return "error";
        default: return "invalid state";
    }
}


const char* description(service_container_base::substates s)
{
    typedef service_container_base::substates states;

    switch(s)
    {
        case states::unstarted: return "unstarted";
        case states::starting: return "starting";
        case states::running: return "running";
        case states::stopping: return "stopping";
        case states::degraded: return "degraded";
        case states::resetting: return "resetting";

        case states::connecting: return "connecting";
        case states::online: return "online";
        case states::offline: return "offline";

        default: return "invalid state";
    }
}