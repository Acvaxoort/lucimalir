//
// Created by rutio on 2020-05-29.
//

#include <sstream>
#include "RangeParameter.h"

RangeParameter::RangeParameter(double value1, double value2)
    : value1(value1), value2(value2) {}

double RangeParameter::getValue(double fraction) {
  return (1.0 - fraction) * value1 + fraction * value2;
}

std::string RangeParameter::tostring() {
  std::stringstream ss;
  ss << "range(" << value1 << ", " << value2 << ")";
  return ss.str();
}
