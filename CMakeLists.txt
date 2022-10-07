cmake_minimum_required(VERSION 3.20)
project(protocol C)

set(CMAKE_C_STANDARD 17)


set(CMOCKA_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/cmocka)

set(CMOCKA_INCLUDE_DIR ${CMOCKA_ROOT_DIR}/include)
set(CMOCKA_STATIC_LIB cmocka)

if (TESTS)
add_definitions(-DUNIT_TESTS)
endif()

enable_testing()
include_directories(include)

add_subdirectory(cmocka)
if (TESTS)
    add_subdirectory(src build/test)
else()
    add_subdirectory(src build/libs)
endif()

add_subdirectory(tests build/tests)


