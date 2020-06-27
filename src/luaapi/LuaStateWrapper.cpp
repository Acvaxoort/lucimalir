//
// Created by rutio on 2020-06-08.
//

#include <stdexcept>
#include <iostream>
#include <lua5.1/lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Vector.h>
#include <core/pipeline/FilterWrapper.h>
#include <core/wrappers/WeakPtrFilterWrapper.h>
#include <core/wrappers/SharedPtrParameter.h>
#include <core/Core.h>
#include <core/pipeline/filters/MandelbrotJulia.h>
#include <core/pipeline/filters/IntegerDataRenderer.h>
#include <core/pipeline/filters/ImageMerge.h>
#include <core/pipeline/filters/ViewInput.h>
#include <core/pipeline/filters/ImageExternalOutput.h>
#include "LuaStateWrapper.h"

LuaStateWrapper::LuaStateWrapper(
    std::ostream* out, std::istream* in, std::ostream* err)
    : out(out), in(in), err(err) {
  prepareState();
}

LuaStateWrapper::~LuaStateWrapper() {
  lua_close(L);
}

void LuaStateWrapper::reset() {
  lua_close(L);
  prepareState();
}

void LuaStateWrapper::doFile(const char* filename) {
  if (luaL_loadfile(L, filename) != LUA_OK) {
    throw std::runtime_error(lua_tostring(L, -1));
  } else if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != LUA_OK) {
    throw std::runtime_error(lua_tostring(L, -1));
  }
  err->flush();
}

void LuaStateWrapper::doString(const std::string& str) {
  std::string new_str = "better_print(" + str + ")";
  if (luaL_loadstring(L, new_str.c_str()) != LUA_OK) {
    lua_pop(L, 1);
    if (luaL_loadstring(L, str.c_str()) != LUA_OK) {
      std::string error_msg = lua_tostring(L, -1);
      (*err) << error_msg << "\n";
      err->flush();
      lua_pop(L, 1);
      return;
    }
  }
  lua_pushcfunction(L, lua_traceback);
  lua_insert(L, -2);
  if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != LUA_OK) {
    (*err) << lua_tostring(L, -1) << "\n";
  } else {
    lua_pop(L, -1);
  }
  err->flush();
}

static void _print_table(lua_State* L, std::ostream& os, int depth = 0) {
  os << "{\n";
  lua_pushnil(L);
  depth += 2;
  while (lua_next(L, -2) != 0) {
    const char* key = lua_tostring(L, -2);
    if (*key != '_') {
      for (int i = 0; i < depth; ++i) {
        os << ' ';
      }
      os << key << " = ";
      if (lua_isstring(L, -1)) {
        os << "\"" << lua_tostring(L, -1) << "\"";
      } else if (lua_isnumber(L, -1)) {
        os << lua_tonumber(L, -1);
      } else if (lua_istable(L, -1)) {
        _print_table(L, os, depth);
      } else {
        os << lua_typename(L, lua_type(L, -1));
      }
      os << ",\n";
    }
    lua_pop(L, 1);
  }
  for (int i = 0; i < depth - 2; ++i) {
    os << ' ';
  }
  os << '}';
}

static int print_table(lua_State* L) {
  lua_settop(L, 1);
  std::stringstream ss;
  _print_table(L, ss, 0);
  lua_pushstring(L, ss.str().c_str());
  return 1;
}

static void print_stack_types(lua_State* L) {
  int top = lua_gettop(L);
  for (int i = 1; i <= top; ++i) {
    std::cout << luaL_typename(L, i) << " ";
  }
  std::cout << "\n";
}

using namespace luabridge;

template<class T>
inline Filter* (* new_ptr_func())() {
  return []() {
    return (Filter*) new T();
  };
}

// clion auto formatting shits itself if I put this in the luabridge calls
typedef void* (* ViewWrapperConstructor2)(
    const PositionWrapper&, const RotationWrapper&, double);

typedef void* (* VectorDoubleConstructor)(const std::vector<double>&);

void LuaStateWrapper::prepareState() {
  L = luaL_newstate();
  if (!L) {
    throw std::runtime_error("Could not create lua state");
  }
  luaL_openlibs(L);
  lua_pushstring(L, "out");
  lua_pushlightuserdata(L, (void*) out);
  lua_settable(L, LUA_REGISTRYINDEX);
  lua_pushstring(L, "err");
  lua_pushlightuserdata(L, (void*) err);
  lua_settable(L, LUA_REGISTRYINDEX);
  lua_pushcfunction(L, &lua_print);
  lua_setglobal(L, "print");

  getGlobalNamespace(L)
      .beginNamespace("_cpp")
      .addCFunction("print_table", &print_table)
      .beginClass<Filter::ParamChangeStatus>("param_change_status")
      .addProperty("change_input", &Filter::ParamChangeStatus::change_input)
      .addProperty("error", &Filter::ParamChangeStatus::error)
      .endClass()
      .beginClass<BufferCollection>("buffer_collection")
      .endClass()
      .beginClass<Buffer>("buffer")
      .endClass()
      .beginClass<Filter>("filter")
      .addFunction("get_name", &Filter::getName)
      .addFunction("get_type_name", &Filter::getTypeName)
      .addFunction("has_number_parameter", &Filter::hasNumberParameter)
      .addFunction("has_string_parameter", &Filter::hasStringParameter)
      .addFunction("has_view_parameter", &Filter::hasViewParameter)
      .addFunction("has_parameter", &Filter::hasParameter)
      .addFunction("get_number_parameter", &Filter::getNumberParameter)
      .addFunction("get_string_parameter", &Filter::getStringParameter)
      .addFunction("get_view_parameter", &Filter::getViewParameter)
      .addFunction("get_parameter", &Filter::getParameter)
      .addFunction("set_number_parameter", &Filter::setNumberParameter)
      .addFunction("set_string_parameter", &Filter::setStringParameter)
      .addFunction("set_view_parameter", &Filter::setViewParameter)
      .addFunction("set_parameter", &Filter::setParameter)
      .addFunction("wait_until_completion", &Filter::waitTillCompletion)
      .endClass()
      .beginClass<FilterWrapper>("filter_wrapper")
      .addConstructor < void(*)/**/(Filter*) > ()
      .addFunction("get_filter", &FilterWrapper::getFilterRaw)
      .addFunction("is_auto_request", &FilterWrapper::isAutoRequestOnChanges)
      .addFunction("set_auto_request", &FilterWrapper::setAutoRequestOnChanges)
      .addFunction("interrupt", &FilterWrapper::interrupt)
      .addFunction("request_rendering", &FilterWrapper::requestRendering)
      .addFunction("can_connect_to", &FilterWrapper::canConnectTo)
      .addFunction("connect_to", &FilterWrapper::connectTo)
      .addFunction("disconnect_input", &FilterWrapper::disconnectInput)
      .addFunction("disconnect_output", &FilterWrapper::disconnectOutput)
      .addFunction("get_input", &FilterWrapper::getInputBuffer)
      .addFunction("get_output", &FilterWrapper::getInputBuffer)
      .addFunction("notify_filter_internal_changes",
                   &FilterWrapper::notifyFilterInternalChanges)
      .addFunction("request_changing_input",
                   &FilterWrapper::requestChangingInput)
      .endClass()
      .beginClass<WeakPtrFilterWrapper>("weak_ptr_filter_wrapper")
      .addFunction("lock_get", &WeakPtrFilterWrapper::lockGet)
      .endClass()
      .beginClass<SharedPtrParameter>("shared_ptr_parameter")
      .addFunction("exists", &SharedPtrParameter::exists)
      .addFunction("__tostring", &SharedPtrParameter::tostring)
      .endClass()
      .beginClass<Core>("core")
      .addStaticFunction("get_instance", &Core::getInstance)
      .addFunction("get_num_threads", &Core::getNumThreads)
      .addFunction("set_num_threads", &Core::setNumThreads)
      .addFunction("add_filter", &Core::addFilter)
      .addFunction("remove_filter", &Core::removeFilter)
      .addFunction("set_priority", &Core::setPriority)
      .addFunction("get_priority", &Core::getPriority)
      .addFunction("get_filters", &Core::getFilters)
      .addFunction("get_external_input_filters", &Core::getExternalInputFilters)
      .addFunction("get_external_output_filters",
                   &Core::getExternalOutputFilters)
      .addFunction("get_filter_group_roots", &Core::getFilterGroupRoots)
      .addFunction("get_pipeline_roots", &Core::getPipelineRoots)
      .addFunction("get_stray_filters", &Core::getStrayFilters)
      .addFunction("await_completion", &Core::awaitCompletion)
      .addFunction("update_filter_groups", &Core::updateFilterGroups)
      .endClass()
      .beginNamespace("filters")
      .beginNamespace("fractals")
      .addFunction("basic", new_ptr_func<MandelbrotJulia>())
      .endNamespace()
      .beginNamespace("renderers")
      .addFunction("basic", new_ptr_func<IntegerDataRenderer>())
      .endNamespace()
      .beginNamespace("images")
      .addFunction("merge", new_ptr_func<ImageMerge>())
      .endNamespace()
      .beginNamespace("inputs")
      .addFunction("view", new_ptr_func<ViewInput>())
      .endNamespace()
      .beginNamespace("outputs")
      .addFunction("image", new_ptr_func<ImageExternalOutput>())
      .endNamespace()
      .endNamespace()
      .beginNamespace("geom4d")
      .beginClass<PositionWrapper>("pos")
      .addConstructor<VectorDoubleConstructor>()
      .addProperty("values", &PositionWrapper::getValues)
      .addFunction("translate", &PositionWrapper::translate)
      .addFunction("__add", &PositionWrapper::translate)
      .addFunction("__tostring", &PositionWrapper::tostring)
      .endClass()
      .beginClass<RotationWrapper>("rot")
      .addConstructor<VectorDoubleConstructor>()
      .addStaticFunction("XY", &RotationWrapper::XY)
      .addStaticFunction("YZ", &RotationWrapper::YZ)
      .addStaticFunction("ZW", &RotationWrapper::ZW)
      .addStaticFunction("XZ", &RotationWrapper::XZ)
      .addStaticFunction("YW", &RotationWrapper::YW)
      .addStaticFunction("XW", &RotationWrapper::XW)
      .addProperty("values", &RotationWrapper::getValues)
      .addFunction("rotate", &RotationWrapper::rotate)
      .addFunction("__mul", &RotationWrapper::rotate)
      .addFunction("__tostring", &RotationWrapper::tostring)
      .endClass()
      .beginClass<ViewWrapper>("view")
      .addConstructor<ViewWrapperConstructor2>()
      .addProperty("pos", &ViewWrapper::getPosition)
      .addProperty("rot", &ViewWrapper::getRotation)
      .addProperty("scale", &ViewWrapper::getScale)
      .addFunction("set_position", &ViewWrapper::setPosition)
      .addFunction("set_rotation", &ViewWrapper::setRotation)
      .addFunction("set_scale", &ViewWrapper::setScale)
      .addFunction("translate",
                   &ViewWrapper::translateWithRotationScale)
      .addFunction("rotate", &ViewWrapper::rotate)
      .addFunction("rescale", &ViewWrapper::rescale)
      .addFunction("__tostring", &ViewWrapper::tostring)
      .endClass()
      .endNamespace()
      .endNamespace()
      .beginNamespace("params")
      .addFunction("constant", &SharedPtrParameter::constant)
      .addFunction("array", &SharedPtrParameter::array)
      .addFunction("range", &SharedPtrParameter::range)
      .addFunction("range_exp", &SharedPtrParameter::rangeExp)
      .endNamespace();

  lua_pushcfunction(L, lua_traceback);
  if (luaL_loadfile(L, "lucimalir.lua") != LUA_OK) {
    (*err) << lua_tostring(L, -1) << "\n";
    throw std::runtime_error("invalid lua library file");
  } else if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != LUA_OK) {
    (*err) << lua_tostring(L, -1) << "\n";
    throw std::runtime_error("invalid lua library file");
  }
}

int LuaStateWrapper::lua_traceback(lua_State* L) {
  //lua_pushstring(L, "err");
  //lua_gettable(L, LUA_REGISTRYINDEX);
  //auto* err = (std::ostream*) lua_touserdata(L, -1);
  //lua_pop(L, 1);
  //(*err) << lua_tostring(L, -1) << "\n";
  //err->flush();
  //lua_pop(L, 1);
  //luaL_traceback(L, L, nullptr, 1);
  lua_getfield(L, LUA_GLOBALSINDEX, "debug");
  lua_getfield(L, -1, "traceback");
  lua_pushvalue(L, 1);
  lua_pushinteger(L, 2);
  lua_call(L, 2, 1);
  return 1;
}

int LuaStateWrapper::lua_print(lua_State* L) {
  lua_pushstring(L, "out");
  lua_gettable(L, LUA_REGISTRYINDEX);
  auto* out = (std::ostream*) lua_touserdata(L, -1);
  lua_pop(L, 1);
  int n = lua_gettop(L);
  int i;
  lua_getglobal(L, "tostring");
  for (i = 1; i <= n; i++) {
    const char* s;
    lua_pushvalue(L, -1);
    lua_pushvalue(L, i);
    lua_call(L, 1, 1);
    s = lua_tostring(L, -1);
    if (s == NULL)
      return luaL_error(L, LUA_QL("tostring") " must return a string to "
                           LUA_QL("print"));
    if (i > 1) {
      (*out) << "\t";
    }
    (*out) << s;
    lua_pop(L, 1);
  }
  (*out) << "\n";
  out->flush();
  return 0;
}
