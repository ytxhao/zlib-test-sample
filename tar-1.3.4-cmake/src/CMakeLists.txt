cmake_minimum_required(VERSION 3.16)
project(tar-test-sample)
set(CMAKE_MACOSX_RPATH 0)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE ON)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../gnu")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
#add_definitions(-DHAVE_CONFIG_H)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/usr/local/include -L/usr/local/lib")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include -L/usr/local/lib -lintl")
#add_subdirectory(gnu)
#add_subdirectory(lib)
#add_subdirectory(src)
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")
#message("-------------------- ${CMAKE_CURRENT_SOURCE_DIR}/tar-1.3.4-cmake")
set(SOURCE_TAR_SRC_FILES
        buffer.c
        checkpoint.c
        compare.c
        create.c
        delete.c
        exclist.c
        exit.c
        extract.c
        incremen.c
        list.c
        map.c
        misc.c
        names.c
        sparse.c
        suffix.c
        system.c
        tar.c
        transform.c
        unlink.c
        update.c
        utf8.c
        warning.c
        xattrs.c
        xheader.c
        )
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/minizip)
#add_library(tar STATIC ${SOURCE_TAR_LIB_FILES})
add_executable(tar-test-sample ${SOURCE_TAR_SRC_FILES})
target_link_libraries(tar-test-sample tar gnu)
#
#target_link_libraries(zlib-test-sample mini-zip)


