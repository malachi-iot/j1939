set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)

include(${ROOT_DIR}/tools/cmake/sources.cmake)

idf_component_register(
                    SRCS ${SOURCE_FILES}
                    INCLUDE_DIRS ${ROOT_DIR}
                    REQUIRES embr estdlib
                    esp_wifi    #DEBT
                    )
