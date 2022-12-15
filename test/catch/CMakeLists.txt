cmake_minimum_required(VERSION 3.0)

project(j1939-unit-test VERSION 1.0.0)

# necessary for Catch2
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DUNIT_TESTING)

set(ROOT_DIR ../..)
set(EXT_DIR ${ROOT_DIR}/ext)
set(SRC_DIR ${ROOT_DIR}/src)

add_subdirectory(${EXT_DIR}/embr/ext/estdlib/ext/Catch2 catch2)
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
    Catch2
    malachi-iot::embr malachi-iot::estd
    can j1939)
