
add_library(cmocka cmocka.c)
target_include_directories(cmocka PUBLIC ${CMOCKA_INCLUDE_DIR})
set_target_properties(cmocka
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)