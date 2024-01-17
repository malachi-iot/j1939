get_filename_component(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)

set(EXT_DIR ${ROOT_DIR}/ext)

set(EMBR_ROOT_DIR ${EXT_DIR}/embr)
set(ESTD_ROOT_DIR ${EXT_DIR}/embr/ext/estdlib)

include(${ESTD_ROOT_DIR}/tools/cmake/fetchcontent.cmake)
