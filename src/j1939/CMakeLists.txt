# Average regular CMakeLists
# esp-idf indicates it is compatible with this:
# https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/api-guides/build-system.html#writing-pure-cmake-components

cmake_minimum_required(VERSION 3.6)

project(j1939 VERSION 0.1.0 LANGUAGES CXX)

include(../lib.cmake)

include(tools/cmake/sources.cmake)

if (NOT TARGET can)
    add_subdirectory(../can can)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(embr::j1939 ALIAS ${PROJECT_NAME})
add_library(malachi-iot::embr::j1939 ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} malachi-iot::estd malachi-iot::embr can)

# 12SEP23 DEBT: WiFi in here was always an experiment, and now it's only used by Chariot
# project who itself is phasing it out
if(ESP_PLATFORM)
    target_link_libraries(${PROJECT_NAME} idf::esp_wifi)
endif()