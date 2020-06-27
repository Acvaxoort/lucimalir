//
// Created by rutio on 2020-06-21.
//

#ifndef LUCIMALIR_SHAREDPTRPARAMETER_H
#define LUCIMALIR_SHAREDPTRPARAMETER_H

#include <memory>
#include <core/pipeline/Parameter.h>
#include <core/pipeline/parameters/ConstantParameter.h>
#include <core/pipeline/parameters/RangeParameter.h>
#include <core/pipeline/parameters/RangeParameterExponential.h>
#include <core/pipeline/parameters/ArrayParameter.h>

class SharedPtrParameter {
public:
  SharedPtrParameter(Parameter* parameter = nullptr)
      : parameter(parameter) {}

  SharedPtrParameter(std::shared_ptr<Parameter> parameter)
      : parameter(std::move(parameter)) {}

  std::string tostring() {
    return parameter.get()->tostring();
  }

  bool exists() {
    return parameter.get();
  }

  Parameter* operator->() {
    return parameter.get();
  }

  std::shared_ptr<Parameter> parameter;

  static SharedPtrParameter constant(double value) {
    return {std::make_shared<ConstantParameter>(value)};
  }

  static SharedPtrParameter range(double v1, double v2) {
    return {std::make_shared<RangeParameter>(v1, v2)};
  }

  static SharedPtrParameter rangeExp(double v1, double v2) {
    return {std::make_shared<RangeParameterExponential>(v1, v2)};
  }

  static SharedPtrParameter array(const std::vector<double>& array) {
    return {std::make_shared<ArrayParameter>(array)};
  }

};


#endif //LUCIMALIR_SHAREDPTRPARAMETER_H
