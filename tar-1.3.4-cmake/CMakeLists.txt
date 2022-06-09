cmake_minimum_required(VERSION 3.16)

set(CMAKE_MACOSX_RPATH 0)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE ON)
add_definitions(-DHAVE_CONFIG_H)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/usr/local/include -L/usr/local/lib")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include -L/usr/local/lib -lintl")
add_subdirectory(gnu)
add_subdirectory(lib)
add_subdirectory(src)
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")
#message("-------------------- ${CMAKE_CURRENT_SOURCE_DIR}/tar-1.3.4-cmake")
#set(SOURCE_TAR_LIB_FILES
#        lib/paxerror.c
#        lib/paxexit-status.c
#        lib/paxnames.c
#        lib/rtapelib.c
#        lib/wordsplit.c
#        lib/xattr-at.c
#        )
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/minizip)
#add_library(tar STATIC ${SOURCE_TAR_LIB_FILES})
#add_executable(zlib-test-sample main.cpp zlib_helper.cc)
#
#target_link_libraries(zlib-test-sample mini-zip)


