cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 14)
include_directories("/Users/yuhaoo/CLionProjects/zlib-1.2.12")
set(SOURCE_FILES
        ioapi.c
        unzip.c
        zip.c
        minizip.c
        miniunz.c
        )
SET(ZLIB_SO /Users/yuhaoo/CLionProjects/zlib-1.2.12/cmake-build-debug/libz.dylib)

add_library(mini-zip STATIC ${SOURCE_FILES})
target_link_libraries(mini-zip ${ZLIB_SO})