# DEBT: Because 'ROOT_DIR' has a different meaning in component.cmake
get_filename_component(ROOT_DIR2 ${CMAKE_CURRENT_LIST_DIR}/../../../.. ABSOLUTE)
set(EMBR_DIR ${ROOT_DIR2}/ext/embr)
