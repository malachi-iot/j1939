#pragma once

#include <estd/port/freertos/queue.h>

#include "ca.h"

class App : transport_type
{
public:
    transport_type& transport() { return *this; }

    // TODO: For gpio notifications
    void on_notify();
};

