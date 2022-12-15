#pragma once

#include "services.h"

namespace embr {

template <class TService, class ...TArgs>
void service_manager::start(TService& s, TArgs...args)
{
    add(s);

    typedef service_container_base::states states;
    typedef service_container_base::substates substates;

    s.state(substates::starting);
    s.start(std::forward<TArgs&&>(args)...);

    if(s.state() == states::stopped && s.substate() == substates::starting)
    {
        s.state(states::started);
        //s.state(states::running);
    }
}

}