cmake_minimum_required(VERSION 3.16)
project(zlib-test-sample)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(minizip)
#include_directories("/Users/yuhaoo/CLionProjects/zlib-1.2.12")
#link_directories("/Users/yuhaoo/CLionProjects/zlib-1.2.12/cmake-build-debug")
#SET(ZLIB_SO /Users/yuhaoo/CLionProjects/zlib-1.2.12/cmake-build-debug/libz.dylib)
add_executable(zlib-test-sample main.cpp)

target_link_libraries(zlib-test-sample mini-zip)