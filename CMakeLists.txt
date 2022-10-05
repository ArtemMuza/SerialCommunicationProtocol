cmake_minimum_required(VERSION 3.23)
project(protocol C)

set(CMAKE_C_STANDARD 17)

enable_testing()
include_directories(include)

add_subdirectory(src build/src)
add_subdirectory(tests build/tests)

