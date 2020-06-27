//
// Created by rutio on 2020-06-09.
//

#ifndef LUCIMALIR_PARAMETERPROXY_H
#define LUCIMALIR_PARAMETERPROXY_H

#include <memory>
#include <core/pipeline/Parameter.h>
#include <core/pipeline/parameters/ConstantParameter.h>
#include <core/pipeline/parameters/RangeParameter.h>
#include <core/pipeline/parameters/RangeParameterExponential.h>
#include <core/pipeline/parameters/ArrayParameter.h>
#include <lua5.1/lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Vector.h>

class ParameterProxy {
public:
  ParameterProxy(std::shared_ptr<Parameter> parameter = nullptr);

  std::string tostring();

  std::shared_ptr<Parameter> parameter;

  static ParameterProxy constant(double value) {
    return {std::make_shared<ConstantParameter>(value)};
  }

  static ParameterProxy range(double v1, double v2) {
    return {std::make_shared<RangeParameter>(v1, v2)};
  }

  static ParameterProxy range_exp(double v1, double v2) {
    return {std::make_shared<RangeParameterExponential>(v1, v2)};
  }

  static ParameterProxy array(const std::vector<double>& array) {
    return {std::make_shared<ArrayParameter>(array)};
  }
};


#endif //LUCIMALIR_PARAMETERPROXY_H
