add_library(lua_lib ../core/wrappers/WeakPtrFilter.h ../core/wrappers/WeakPtrFilterWrapper.h)
target_link_libraries(lua_lib core_lib)