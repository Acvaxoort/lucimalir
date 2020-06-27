//
// Created by rutio on 2020-06-23.
//

#include <stdexcept>
#include <lua5.1/lua.hpp>
#include <iostream>
#include "HelpStrings.h"

HelpStrings& HelpStrings::getinstance() {
  static HelpStrings instance;
  return instance;
}

HelpStrings::HelpStrings() {
  lua_State* L = luaL_newstate();
  if (!L) {
    throw std::runtime_error("Could not create lua state");
  }
  luaL_openlibs(L);
  if (luaL_loadfile(L, "help.lua") != LUA_OK) {
    std::cerr << lua_tostring(L, -1) << "\n";
    throw std::runtime_error("invalid help file");
  } else if (lua_pcall(L, 0, 1, lua_gettop(L) - 1) != LUA_OK) {
    std::cerr << lua_tostring(L, -1) << "\n";
    throw std::runtime_error("invalid help file");
  }
  /*lua_pushnil(L);
  while (lua_next(L, -2) != 0) {
    if (lua_isstring(L, -1)) {

    } else if (lua_istable(L, -1)) {

    } else {
      throw std::runtime_error(std::string("unexpected type: ") + lua_typename(L, lua_type(L, -1)));
    }
    lua_pop(L, 1);
  }*/
  lua_close(L);
}

HelpStrings::Node::operator const std::string&() const {
  return content;
}

const HelpStrings::Node& HelpStrings::Node::get(const std::string& key) const {
  auto it = children.find(key);
  if (it != children.end()) {
    return it->second;
  } else {
    throw std::runtime_error("invalid key");
  }
}
