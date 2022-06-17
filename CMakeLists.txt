cmake_minimum_required(VERSION 3.16)
project(zlib-test-sample)
set(CMAKE_MACOSX_RPATH 0)
set(CMAKE_CXX_STANDARD 14)
# 显示 CMake 输出所有的编译过程
set(CMAKE_VERBOSE_MAKEFILE ON)
add_subdirectory(tar-1.3.4-cmake)
add_subdirectory(zlib-1.2.12)
add_subdirectory(minizip)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/minizip)
#include_directories("/Users/yuhaoo/CLionProjects/zlib-1.2.12")
#link_directories("/Users/yuhaoo/CLionProjects/zlib-1.2.12/cmake-build-debug")
#SET(ZLIB_SO /Users/yuhaoo/CLionProjects/zlib-1.2.12/cmake-build-debug/libz.dylib)
add_executable(zlib-test-sample main.cpp zlib_helper.cc)

target_link_libraries(zlib-test-sample mini-zip tar gnu)

#add_executable(tar-test-sample ${SOURCE_TAR_SRC_FILES})


