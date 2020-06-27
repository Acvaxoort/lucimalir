cmake_minimum_required(VERSION 3.13)
project(lucimalir)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(CLION_OUTPUT_DEBUG_FIX)
endif()

add_compile_definitions(LOGGER_LEVEL=4)
#add_compile_definitions(HEAVY_DEBUG)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

include_directories(src libs)

add_subdirectory(src)
#add_subdirectory(test)