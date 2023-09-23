cmake_minimum_required(VERSION 3.0)

project(j1939-unit-test VERSION 1.0.0)

# necessary for Catch2
if(NOT DEFINED CMAKE_CXX_STANDARD)
    message(STATUS "Forcing to minimum c++11")
    set(CMAKE_CXX_STANDARD 11)
endif()

add_definitions(-DUNIT_TESTING)


include(../../tools/cmake/setvars.cmake)
set(SRC_DIR ${ROOT_DIR}/src)

add_subdirectory(${SRC_DIR}/can can)
add_subdirectory(${SRC_DIR}/j1939 j1939)

Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
    GIT_TAG         v2.13.10
    GIT_PROGRESS    TRUE
    GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(Catch2)

set(SOURCE_FILES
    main.cpp
    bits2-test.cpp
    ca-test.cpp
    dispatcher-test.cpp
    experimental-test.cpp
    ecu-test.cpp
    name-test.cpp
    network-test.cpp
    pdu-test.cpp
    pgn-test.cpp
    ostream-test.cpp
    slots-test.cpp
    spn-test.cpp
    transport-test.cpp
    units-test.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# DEBT: Consolidate this into an estdlib helper cmake, and perhaps even a
# kind of CMake OPTION which defaults to off - that might be even better
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror=return-type)
endif()

target_link_libraries(${PROJECT_NAME}
    Catch2
    malachi-iot::embr malachi-iot::estd
    can j1939)
