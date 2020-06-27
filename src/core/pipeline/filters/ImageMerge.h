//
// Created by rutio on 2020-05-30.
//

#ifndef LUCIMALIR_IMAGEMERGE_H
#define LUCIMALIR_IMAGEMERGE_H

#include <core/util/DataTypes.h>
#include <core/pipeline/Parameter.h>
#include <core/pipeline/parameters/ConstantParameter.h>
#include "NonContextualMergeFilter.h"
#include "NonContextualMergeFilterJob.h"

class ImageMerge
    : public NonContextualMergeFilter<DataTypes::RGBA, DataTypes::RGBA> {
public:
  enum ImageMergeMode {
    ADD_LIMIT,
    ADD_TANH,
    ADD_TANH_LIMIT,
    SUB_LIMIT,
    SUB_TANH,
    SUB_TANH_LIMIT,
    MULTIPLY,
    MULTIPLY_SUB
  };

  struct Params {
    ImageMergeMode mode = ADD_LIMIT;
    SharedPtrParameter multiplier = SharedPtrParameter::constant(1);
    double post_multiplier = 1;
  };

  Params params;

  class Job
      : public NonContextualMergeFilterJob<DataTypes::RGBA, DataTypes::RGBA> {
  public:
    Job(std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output);

    void completeNonContextualMerge(
        MatrixLayerArrayView<DataTypes::RGBA> input,
        MatrixLayerArrayView<DataTypes::RGBA> output) override;

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

  ParamChangeStatus
  setStringParameter(const std::string& key, const std::string& value) override;

  ParamChangeStatus
  setParameter(const std::string& key, SharedPtrParameter value) override;

protected:
  NonContextualMergeFilterJob<DataTypes::RGBA, DataTypes::RGBA>*
  makeNextJobNonContextualMerge(
      std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output) override;
};


#endif //LUCIMALIR_IMAGEMERGE_H
