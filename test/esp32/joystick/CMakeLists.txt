cmake_minimum_required(VERSION 3.5)

add_compile_definitions("FEATURE_EMBR_ESP_LEGACY_DEBOUNCE=0")
add_compile_definitions("FEATURE_EMBR_ESP_TIMER_SCHEDULER=0")

include(../setenv.cmake)

project(j1939-joystick)