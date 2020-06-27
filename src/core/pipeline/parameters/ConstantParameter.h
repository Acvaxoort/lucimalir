//
// Created by rutio on 2020-05-29.
//

#ifndef LUCIMALIR_CONSTANTPARAMETER_H
#define LUCIMALIR_CONSTANTPARAMETER_H


#include "core/pipeline/Parameter.h"

class ConstantParameter : public Parameter {
public:
  ConstantParameter(double value);

  double getValue(double fraction) override;

  std::string tostring() override;

  double value;
};


#endif //LUCIMALIR_CONSTANTPARAMETER_H
