if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_NAME lucimalir_debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(EXECUTABLE_NAME lucimalir)
endif()

add_executable(${EXECUTABLE_NAME} main.cpp ../luaapi/LuaStateWrapper.cpp ../luaapi/LuaStateWrapper.h)
target_link_libraries(${EXECUTABLE_NAME} lua_lib)
target_link_libraries(${EXECUTABLE_NAME} Threads::Threads)
target_link_libraries(${EXECUTABLE_NAME} lua5.1)

#if(UNIX)
#    target_link_libraries(${EXECUTABLE_NAME} ${CMAKE_DL_LIBS})
#    target_link_libraries(${EXECUTABLE_NAME} ${X11_LIBRARIES})
#endif()