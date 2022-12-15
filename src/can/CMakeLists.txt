cmake_minimum_required(VERSION 3.6)

project(can VERSION 0.0.2 LANGUAGES CXX)

include(../lib.cmake)

include(tools/cmake/sources.cmake)

#add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
#target_include_directories(${PROJECT_NAME} PUBLIC .)

# "INTERFACE library can only be used with the INTERFACE keyword of
#  target_link_libraries"
add_library(${PROJECT_NAME} INTERFACE)
add_library(embr::can ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE .)

target_link_libraries(${PROJECT_NAME} INTERFACE estd malachi-iot::embr)

# It seems only properties prefixed with INTERFACE_ are permitted
# (https://stackoverflow.com/questions/68502038/custom-properties-for-interface-libraries)
#set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)