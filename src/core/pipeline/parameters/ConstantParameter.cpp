//
// Created by rutio on 2020-05-29.
//

#include <sstream>
#include "ConstantParameter.h"

ConstantParameter::ConstantParameter(double value): value(value) {}

double ConstantParameter::getValue(double fraction) {
  return value;
}

std::string ConstantParameter::tostring() {
  std::stringstream ss;
  ss << "constant(" << value << ")";
  return ss.str();
}
