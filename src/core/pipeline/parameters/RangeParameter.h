//
// Created by rutio on 2020-05-29.
//

#ifndef LUCIMALIR_RANGEPARAMETER_H
#define LUCIMALIR_RANGEPARAMETER_H

#include "core/pipeline/Parameter.h"

class RangeParameter : public Parameter {
public:
  RangeParameter(double value1, double value2);

  double getValue(double fraction) override;

  std::string tostring() override;

  double value1;
  double value2;
};


#endif //LUCIMALIR_RANGEPARAMETER_H
