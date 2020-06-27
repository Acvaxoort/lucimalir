//
// Created by rutio on 2020-05-29.
//

#ifndef LUCIMALIR_PARAMETER_H
#define LUCIMALIR_PARAMETER_H

#include <string>

class Parameter {
public:
  double getValue(int index, int count);

  virtual double getValue(double fraction) = 0;

  virtual std::string tostring() = 0;

  virtual ~Parameter() = default;
};


#endif //LUCIMALIR_PARAMETER_H
