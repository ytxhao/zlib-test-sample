cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 14)

message("====== CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message("====== PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("====== CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/zlib-1.2.12")
set(SOURCE_FILES
        ioapi.c
        unzip.c
        zip.c
        )
#SET(ZLIB_SO /Users/yuhaoo/CLionProjects/zlib-1.2.12/cmake-build-debug/libz.dylib)

add_library(mini-zip STATIC ${SOURCE_FILES})
target_link_libraries(mini-zip zlib)

add_executable(minizip-app minizip.c )
target_link_libraries(minizip-app mini-zip)

add_executable(miniunz-app miniunz.c)
target_link_libraries(miniunz-app mini-zip)