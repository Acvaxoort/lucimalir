//
// Created by rutio on 2020-06-09.
//

#include "ParameterProxy.h"

ParameterProxy::ParameterProxy(std::shared_ptr<Parameter> parameter)
    : parameter(std::move(parameter)) {}

std::string ParameterProxy::tostring() {
  return parameter->tostring();
}
