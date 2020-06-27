//
// Created by rutio on 2020-06-08.
//

#include "RangeParameterExponential.h"

#include <cmath>
#include <string>
#include <sstream>

RangeParameterExponential::RangeParameterExponential(
    double value1, double value2) {
  if (value1 <= 0.0 || value2 <= 0.0) {
    throw std::runtime_error("expected positive numbers");
  }
  log_value1 = std::log(value1);
  log_value2 = std::log(value2);
}

double RangeParameterExponential::getValue(double fraction) {
  return std::exp((1.0 - fraction) * log_value1 + fraction * log_value2);
}

std::string RangeParameterExponential::tostring() {
  std::stringstream ss;
  ss << "range_exp(" << std::exp(log_value1)
     << ", " << std::exp(log_value2) << ")";
  return ss.str();
}
