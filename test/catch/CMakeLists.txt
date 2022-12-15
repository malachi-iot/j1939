cmake_minimum_required(VERSION 3.0)

project(j1939-unit-test VERSION 1.0.0)

# necessary for Catch2
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DUNIT_TESTING)

set(ROOT_DIR ../..)
set(EXT_DIR ${ROOT_DIR}/ext)
set(SRC_DIR ${ROOT_DIR}/src)
set(CATCH_DIR ${EXT_DIR}/embr/ext/Catch2/single_include/catch2)

include_directories(${CATCH_DIR})

add_subdirectory(${EXT_DIR}/embr/src embr)

add_subdirectory(${SRC_DIR}/can can)
add_subdirectory(${SRC_DIR}/j1939 j1939)

set(SOURCE_FILES
    main.cpp
    bits2-test.cpp
    ca-test.cpp
    dispatcher-test.cpp
    experimental-test.cpp
    ecu-test.cpp
    name-test.cpp
    pdu-test.cpp
    pgn-test.cpp
    slots-test.cpp
    spn-test.cpp
    transport-test.cpp
    units-test.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    malachi-iot::embr malachi-iot::estd
    can j1939)
