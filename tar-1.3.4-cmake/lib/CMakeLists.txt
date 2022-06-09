cmake_minimum_required(VERSION 3.16)

set(CMAKE_MACOSX_RPATH 0)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE ON)

#add_definitions(-DHAVE_CONFIG_H)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../gnu")
message("-------------------- ${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCE_TAR_LIB_FILES
        paxerror.c
        paxexit-status.c
        paxnames.c
        rtapelib.c
        wordsplit.c
        xattr-at.c
        )
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/minizip)
add_library(tar STATIC ${SOURCE_TAR_LIB_FILES})
#add_executable(zlib-test-sample main.cpp zlib_helper.cc)
#
#target_link_libraries(zlib-test-sample mini-zip)


