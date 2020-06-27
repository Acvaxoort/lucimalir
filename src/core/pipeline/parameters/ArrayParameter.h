//
// Created by rutio on 2020-06-08.
//

#ifndef LUCIMALIR_ARRAYPARAMETER_H
#define LUCIMALIR_ARRAYPARAMETER_H

#include <vector>
#include "core/pipeline/Parameter.h"

class ArrayParameter : public Parameter {
public:
  ArrayParameter(std::vector<double> elements);

  double getValue(double fraction) override;

  std::string tostring() override;

private:
  std::vector<double> elements;
};


#endif //LUCIMALIR_ARRAYPARAMETER_H
