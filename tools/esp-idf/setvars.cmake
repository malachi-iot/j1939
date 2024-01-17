include(${CMAKE_CURRENT_LIST_DIR}/../cmake/setvars.cmake)

set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS}"
    "${ROOT_DIR}/src/j1939/tools/esp-idf/components"
    "${EMBR_ROOT_DIR}/tools/esp-idf/components"
    
    # DEBT: This test area includes a 'testable' too which we do NOT want, but doesn't appear
    # to hurt our j1939 test apps since we're not doing any unity stuff here YET....
    "${EMBR_ROOT_DIR}/test/rtos/esp-idf/components"
    )

# DEBT: Still doing it this way instead of component dir because version_finder
# likes it.  That said, version_finder is most definitely deprecated - but parts
# of estd (IIRC) still depend on it
include(${ESTD_ROOT_DIR}/tools/esp-idf/project.cmake)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

add_compile_definitions(WORKAROUND_EMBR_MAYBE_UNINITIALIZED=1)
