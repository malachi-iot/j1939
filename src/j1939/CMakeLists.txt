# Average regular CMakeLists
# esp-idf indicates it is compatible with this:
# https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/api-guides/build-system.html#writing-pure-cmake-components

cmake_minimum_required(VERSION 3.6)

project(j1939)

include(../lib.cmake)

include(tools/cmake/sources.cmake)

if (NOT TARGET can)
    add_subdirectory(${LIB_DIR}/can can)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} malachi-iot::estd malachi-iot::embr can)