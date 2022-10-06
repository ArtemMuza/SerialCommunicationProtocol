cmake_minimum_required(VERSION 3.23)
project(protocol C)

set(CMAKE_C_STANDARD 17)


set(CMOCKA_ROOT_DIR "C:/Program Files (x86)/cmocka")

set(CMOCKA_INCLUDE_DIR ${CMOCKA_ROOT_DIR}/include)
set(CMOCKA_STATIC_DYNAMIC ${CMOCKA_ROOT_DIR}/bin/cmocka.dll)
#set(CMOCKA_STATIC_LIB ${CMOCKA_ROOT_DIR}/lib/libcmocka.dll.a)
set(CMOCKA_STATIC_LIB cmocka)

enable_testing()
include_directories(include)

add_subdirectory(src build/src)
add_subdirectory(tests build/tests)

