add_library(lua_lib LuaStateWrapper.cpp LuaStateWrapper.h)
target_link_libraries(lua_lib core_lib)