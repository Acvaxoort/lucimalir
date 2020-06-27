//
// Created by rutio on 2020-05-30.
//

#ifndef LUCIMALIR_VIEWINPUT_H
#define LUCIMALIR_VIEWINPUT_H

#include <core/pipeline/Filter.h>

class ViewInput : public Filter {
public:
  struct Params {
    RenderingView<double> view;
    DataTypes::FloatingPrecision precision = DataTypes::FP64;
  };

  Params params;

  class Job : public FilterJob {
  public:
    Job(std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output);

    void complete() override;

    Params params;
  };

  std::unique_ptr<Filter> clone() const final;

  double getProgress() const final;

  OptionalError canOutputTo(Buffer* output) const final;

  OptionalError adaptToOutput(Buffer* output, FilterWrapper* context) final;

  OptionalError canInputFrom(Buffer* input) const final;

  OptionalError adaptToInput(Buffer* input) final;

  std::shared_ptr<Buffer> makeInputBuffer() const final;

  bool hasExternalInput() const final;

  const char* getLabel() const final;

  const char* getTypeName() const final;

  bool hasNumberParameter(const std::string& key) override;

  bool hasViewParameter(const std::string& key) override;

  double getNumberParameter(const std::string& key) override;

  ViewWrapper getViewParameter(const std::string& key) override;

  ParamChangeStatus
  setNumberParameter(const std::string& key, double value) override;

  ParamChangeStatus
  setViewParameter(const std::string& key, const ViewWrapper& value) override;

protected:
  FilterJob* makeNextJob(
      std::shared_ptr<Buffer> input,
      std::shared_ptr<Buffer> output) final;

private:
  bool did_output = false;
};


#endif //LUCIMALIR_VIEWINPUT_H
