if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_NAME lucimalir_gui_debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(EXECUTABLE_NAME lucimalir_gui)
endif()

add_executable(${EXECUTABLE_NAME} main.cpp)
target_link_libraries(${EXECUTABLE_NAME} lua_lib)
target_link_libraries(${EXECUTABLE_NAME} Threads::Threads)
target_link_libraries(${EXECUTABLE_NAME} lua5.1)
