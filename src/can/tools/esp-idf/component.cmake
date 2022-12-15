set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)

include(${ROOT_DIR}/tools/cmake/sources.cmake)

# as per
# https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/api-guides/build-system.html
# we can set source dir also
idf_component_register(
                    # DEBT: Prefer SRCS pointing at exact files.  SRC_DIRS is globbing
                    #SRCS ${SOURCE_FILES}
                    INCLUDE_DIRS ${ROOT_DIR}
                    REQUIRES embr estdlib)
