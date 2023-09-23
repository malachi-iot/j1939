get_filename_component(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)

if (NOT TARGET malachi-iot::estd)
    add_subdirectory(${ROOT_DIR}/ext/embr/ext/estdlib/src estdlib)
endif()

if (NOT TARGET malachi-iot::embr)
    add_subdirectory(${ROOT_DIR}/ext/embr/src embr)
endif()

