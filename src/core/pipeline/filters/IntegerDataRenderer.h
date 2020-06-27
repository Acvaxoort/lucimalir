//
// Created by rutio on 2020-05-30.
//

#ifndef LUCIMALIR_INTEGERDATARENDERER_H
#define LUCIMALIR_INTEGERDATARENDERER_H


#include <core/util/DataTypes.h>
#include <core/pipeline/Parameter.h>
#include <core/util/Colors.h>
#include "NonContextualFilter.h"
#include "NonContextualFilterJob.h"

class IntegerDataRenderer
    : public NonContextualFilter<DataTypes::INTEGER, DataTypes::RGBA> {
public:
  struct Params {
    Params();

    void set_default_RGB();

    void set_default_HSV();

    SharedPtrParameter low_par1;
    SharedPtrParameter low_par2;
    SharedPtrParameter low_par3;
    SharedPtrParameter high_par1;
    SharedPtrParameter high_par2;
    SharedPtrParameter high_par3;
    SharedPtrParameter input_gamma;
    SharedPtrParameter output_gamma;
    Colors::ColorModel color_model;
  };

  Params params;

  class Job
      : public NonContextualFilterJob<DataTypes::INTEGER, DataTypes::RGBA> {
  public:
    Job(std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output);

    void completeNonContextual(
        MatrixLayerArrayView<DataTypes::INTEGER> input,
        MatrixLayerArrayView<DataTypes::RGBA> output) final;

    Params params;
  };

  std::unique_ptr<Filter> clone() const final;

  const char* getTypeName() const final;

  const char* getLabel() const final;

  bool hasNumberParameter(const std::string& key) override;

  bool hasStringParameter(const std::string& key) override;

  bool hasParameter(const std::string& key) override;

  double getNumberParameter(const std::string& key) override;

  std::string getStringParameter(const std::string& key) override;

  SharedPtrParameter getParameter(const std::string& key) override;

  ParamChangeStatus
  setNumberParameter(const std::string& key, double value) override;

  Filter::ParamChangeStatus
  setStringParameter(const std::string& key, const std::string& value) override;

  Filter::ParamChangeStatus
  setParameter(const std::string& key, SharedPtrParameter value) override;

protected:
  NonContextualFilterJob<DataTypes::INTEGER, DataTypes::RGBA>*
  makeNextJobNonContextual(
      std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output) override;

};


#endif //LUCIMALIR_INTEGERDATARENDERER_H
