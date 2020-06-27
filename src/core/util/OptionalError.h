//
// Created by rutio on 2020-04-09.
//

#ifndef LUCIMALIR_OPTIONALERROR_H
#define LUCIMALIR_OPTIONALERROR_H

#include <string>
#include <functional>
#include <lua5.1/lua.hpp>
#include <LuaBridge/LuaBridge.h>

struct OptionalError {
  inline OptionalError(bool success = true)
      : success(success) {}

  inline OptionalError(const char* message)
      : success(false), message(message) {}

  inline OptionalError(std::string message)
      : success(false), message(std::move(message)) {}

  inline OptionalError prependIfError(const std::string& prepend_message) {
    return success ? OptionalError(true)
                   : OptionalError(prepend_message + message);
  }

  using StringFunc = std::function<std::string()>;

  inline OptionalError prependIfError(const StringFunc& func) {
    return success ? OptionalError(true)
                   : OptionalError(func() + message);
  }

  inline void accumulate(OptionalError&& other, const char* separator = "; ") {
    if (other.success) {
      return;
    }
    if (success) {
      success = false;
      message = std::move(other.message);
    } else {
      message += separator;
      message += other.message;
    }
  }

  bool success;
  std::string message;
};

template<>
struct luabridge::Stack<OptionalError> {
  static void push(lua_State* L, const OptionalError& status) {
    if (status.success) {
      lua_pushboolean(L, false);
    } else {
      lua_pushstring(L, status.message.c_str());
    }
  }

  static OptionalError get(lua_State* L, int index) {
    if (lua_isboolean(L, index)) {
      return true;
    } else {
      luaL_checkstring(L, index);
    }
  }

  static bool isInstance(lua_State* L, int index) {
    auto type = lua_type(L, index);
    return type == LUA_TSTRING || type == LUA_TBOOLEAN;
  }
};


#endif //LUCIMALIR_OPTIONALERROR_H
