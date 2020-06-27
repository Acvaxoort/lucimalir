//
// Created by rutio on 2020-06-09.
//

#ifndef LUCIMALIR_FILTERPARAMSPROXYBASE_H
#define LUCIMALIR_FILTERPARAMSPROXYBASE_H

#include <memory>
#include <core/pipeline/Filter.h>
#include <lua5.1/lua.hpp>
#include <LuaBridge/LuaBridge.h>


class FilterParamsProxy {
public:
  explicit FilterParamsProxy(std::weak_ptr<FilterWrapper> wrapper);

  OptionalError getLastError();
/*
  static int index(lua_State* L);

  static int newindex(lua_State* L);
*/
private:
  FilterWrapper* getWrapper();

  OptionalError last_error;
  std::weak_ptr<FilterWrapper> wrapper;
};


#endif //LUCIMALIR_FILTERPARAMSPROXYBASE_H
