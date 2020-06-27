//
// Created by rutio on 2020-05-29.
//

#include "Parameter.h"

double Parameter::getValue(int index, int count) {
  if (count > 1) {
    return getValue(index / (count - 1.0));
  } else {
    return getValue(0.5);
  }
}
