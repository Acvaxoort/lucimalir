//
// Created by rutio on 2020-06-08.
//

#include <cmath>
#include <sstream>
#include "ArrayParameter.h"

ArrayParameter::ArrayParameter(std::vector<double> elements)
    : elements(std::move(elements)) {}

double ArrayParameter::getValue(double fraction) {
  int index = (int) std::round(fraction * elements.size());
  index = std::max(std::min(index, (int) elements.size() - 1), 0);
  return elements[index];
}

std::string ArrayParameter::tostring() {
  std::stringstream ss;
  ss << "array{";
  for (int i = 0; i < elements.size(); ++i) {
    if (i > 0) {
      ss << ", ";
    }
    ss << elements[i];
  }
  ss << "}";
  return ss.str();
}
