//
// Created by rutio on 2020-06-08.
//

#ifndef LUCIMALIR_LUASTATEWRAPPER_H
#define LUCIMALIR_LUASTATEWRAPPER_H

#include <lua5.1/lua.hpp>

class LuaStateWrapper {
public:
  LuaStateWrapper(std::ostream* out, std::istream* in, std::ostream* err);

  ~LuaStateWrapper();

  void reset();

  void doFile(const char* filename);

  void doString(const std::string& str);

private:
  void prepareState();
  static int lua_traceback(lua_State* L);
  static int lua_print(lua_State* L);

  lua_State* L;
  std::ostream* out;
  std::istream* in;
  std::ostream* err;
};


#endif //LUCIMALIR_LUASTATEWRAPPER_H
