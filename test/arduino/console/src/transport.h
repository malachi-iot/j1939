#pragma once


#ifdef AUTOWP_LIB
#include <can/platform/arduino/autowp_transport.h>
using transport = embr::can::autowp_transport;
#elif defined(ESP_PLATFORM)
#include <can/platform/esp-idf/transport.hpp>
using transport = embr::can::esp_idf::twai_transport<false>;
#else
#include <can/platform/arduino/adafruit_transport.h>
using transport = embr::can::adafruit_transport;
#endif

void init_can(transport& t);

extern bool can_online;
