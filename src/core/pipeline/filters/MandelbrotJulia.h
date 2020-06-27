//
// Created by rutio on 2020-04-23.
//

#ifndef LUCIMALIR_MANDELBROTJULIA_H
#define LUCIMALIR_MANDELBROTJULIA_H


#include <core/pipeline/Filter.h>
#include <core/pipeline/parameters/ConstantParameter.h>
#include <core/wrappers/SharedPtrParameter.h>

class MandelbrotJulia : public Filter {
public:
  struct Params {
    DataTypes::FloatingPrecision precision = DataTypes::FP64;
    SharedPtrParameter iterations = {std::make_shared<ConstantParameter>(64)};
  };

  Params params;

  class Job : public FilterJob {
  public:
    Job(std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output);

    void complete() override;

    int segment_index;

    Params params;
  };

  double getProgress() const override;

  std::unique_ptr<Filter> clone() const override;

  OptionalError canOutputTo(Buffer* output) const override;

  OptionalError adaptToOutput(Buffer* output, FilterWrapper* context) override;

  OptionalError canInputFrom(Buffer* input) const override;

  OptionalError adaptToInput(Buffer* input) override;

  std::shared_ptr<Buffer> makeInputBuffer() const override;

  const char* getTypeName() const override;

  const char* getLabel() const override;

  bool hasNumberParameter(const std::string& key) override;

  bool hasParameter(const std::string& key) override;

  double getNumberParameter(const std::string& key) override;

  SharedPtrParameter getParameter(const std::string& key) override;

  Filter::ParamChangeStatus
  setNumberParameter(const std::string& key, double value) override;

  Filter::ParamChangeStatus
  setParameter(const std::string& key, SharedPtrParameter value) override;

protected:
  FilterJob* makeNextJob(
      std::shared_ptr<Buffer> input,
      std::shared_ptr<Buffer> output) final;

private:
  int next_segment = 0;
  int num_segments = 0;
};


#endif //LUCIMALIR_MANDELBROTJULIA_H
