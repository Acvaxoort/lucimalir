//
// Created by rutio on 2020-06-09.
//

#include "FilterParamsProxy.h"
#include "ParameterProxy.h"

/*
FilterParamsProxy::FilterParamsProxy(
    std::weak_ptr<FilterWrapper> wrapper)
    : wrapper(std::move(wrapper)) {
}

OptionalError FilterParamsProxy::getLastError() {
  return last_error;
}

FilterWrapper* FilterParamsProxy::getWrapper() {
  if (wrapper.expired()) {
    throw std::runtime_error("invalid filter object");
  } else {
    return wrapper.lock().get();
  }
}

int FilterParamsProxy::index(lua_State* L) {
  luabridge::LuaRef table = luabridge::LuaRef::fromStack(L, 1);
  auto this_ = table.cast<FilterParamsProxy*>();
  auto filter_weak = this_->getWrapper()->getFilter();
  //auto filter_weak = getWrapper()->getFilter();
  std::string key = luaL_checkstring(L, 2);
  if (filter_weak.expired()) {
    luaL_error(L, "Filter is invalid");
  }
  auto filter = filter_weak.lock();
  if (auto ret_number = filter->getNumberParameter(key)) {
    lua_pushnumber(L, ret_number.value());
    return 1;
  }
  if (auto ret_string = filter->getStringParameter(key)) {
    lua_pushstring(L, ret_string.value().c_str());
    return 1;
  }
  if (auto ret_par = filter->getParameter(key)) {
    luabridge::LuaRef ret(L, ParameterProxy(std::move(ret_par)));
    ret.push(L);
    return 1;
  }
  luaL_error(L, "Parameter not found");
  return 0;
}

int FilterParamsProxy::newindex(lua_State* L) {
  luabridge::LuaRef table = luabridge::LuaRef::fromStack(L, 1);
  auto this_ = table.cast<FilterParamsProxy*>();
  auto filter_weak = this_->getWrapper()->getFilter();
  //auto filter_weak = getWrapper()->getFilter();
  std::string key = luaL_checkstring(L, 2);
  if (filter_weak.expired()) {
    luaL_error(L, "Filter is invalid");
  }
  auto filter = filter_weak.lock();
  OptionalError status;
  if (lua_isnumber(L, 3)) {
    status = filter->setNumberParameter(key, lua_tonumber(L, 3));
  } else if (lua_isstring(L, 3)) {
    status = filter->setStringParameter(key, lua_tostring(L, 3));
  } else {
    luabridge::LuaRef value(L, 3);
    auto par = value.cast<ParameterProxy>();
    status = filter->setParameter(key, par.parameter);
  }
  if (status.success) {
    return 0;
  } else {
    //luaL_error(L, status.message.c_str());
    //luabridge::
    //throw std::invalid_argument(status.message);
    return 0;
  }
}
*/