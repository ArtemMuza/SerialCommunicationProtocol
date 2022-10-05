cmake_minimum_required(VERSION 3.23)
project(protocol C)

set(CMAKE_C_STANDARD 17)

set(CMOCKA_ROOT_DIR "C:/Program Files (x86)/cmocka")

find_path(CMOCKA_INCLUDE_DIR NAMES cmocka.h PATHS ${CMOCKA_ROOT_DIR}/include)

find_library(CMOCKA_LIBRARY NAMES cmocka cmocka_shared PATHS ${CMOCKA_ROOT_DIR}/include)

if (CMOCKA_LIBRARY)
    set(CMOCKA_LIBRARIES ${CMOCKA_LIBRARIES} ${CMOCKA_LIBRARY} )
endif (CMOCKA_LIBRARY)

enable_testing()
include_directories(include)

add_subdirectory(src build/src)
add_subdirectory(tests build/tests)

