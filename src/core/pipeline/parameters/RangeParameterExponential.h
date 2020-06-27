//
// Created by rutio on 2020-06-08.
//

#ifndef LUCIMALIR_RANGEPARAMETEREXPONENTIAL_H
#define LUCIMALIR_RANGEPARAMETEREXPONENTIAL_H

#include "core/pipeline/Parameter.h"

class RangeParameterExponential : public Parameter {
public:
  RangeParameterExponential(double value1, double value2);

  double getValue(double fraction) override;

  std::string tostring() override;

  double log_value1;
  double log_value2;
};


#endif //LUCIMALIR_RANGEPARAMETEREXPONENTIAL_H
