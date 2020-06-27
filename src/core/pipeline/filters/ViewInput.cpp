//
// Created by rutio on 2020-05-30.
//

#include "ViewInput.h"

std::unique_ptr<Filter> ViewInput::clone() const {
  return std::make_unique<ViewInput>(*this);
}

double ViewInput::getProgress() const {
  return did_output ? 1.0 : 0.0;
}


OptionalError ViewInput::canOutputTo(Buffer* output) const {
  auto status = Filter::verifyViewBufferOfFloat(output);
  return status;
}

OptionalError ViewInput::adaptToOutput(Buffer* output, FilterWrapper* context) {
  params.precision = Buffer::getPrecision(output).value();
  return true;
}

OptionalError ViewInput::canInputFrom(Buffer* input) const {
  return "this filter doesn't input";
}

OptionalError ViewInput::adaptToInput(Buffer* input) {
  return true;
}

std::shared_ptr<Buffer> ViewInput::makeInputBuffer() const {
  return nullptr;
}

bool ViewInput::hasExternalInput() const {
  return true;
}

const char* ViewInput::getTypeName() const {
  return "view_input";
}

const char* ViewInput::getLabel() const {
  return "View Input";
}

FilterJob* ViewInput::makeNextJob(
    std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output) {
  if (!did_output) {
    did_output = true;
    auto ret = new Job(std::move(input), std::move(output));
    ret->params = params;
    return ret;
  }
  return nullptr;
}

bool ViewInput::hasNumberParameter(const std::string& key) {
  FILTER_HAS(precision) //
  FILTER_HAS_END()
}

bool ViewInput::hasViewParameter(const std::string& key) {
  FILTER_HAS(view) //
  FILTER_HAS_END()
}

double ViewInput::getNumberParameter(const std::string& key) {
  FILTER_GET_ENUM(precision) //
    FILTER_GET_ENUM_VALUE(32, DataTypes::FP32) //
    FILTER_GET_ENUM_VALUE(64, DataTypes::FP64) //
  FILTER_GET_ENUM_END(DataTypes::FP64) //
  FILTER_GET_END_NUM()
}

ViewWrapper ViewInput::getViewParameter(const std::string& key) {
  FILTER_GET(view) //
  FILTER_GET_END_VIEW()
}

Filter::ParamChangeStatus
ViewInput::setNumberParameter(const std::string& key, double value) {
    FILTER_SET_ENUM(precision, true) //
      FILTER_SET_ENUM_VALUE(32, DataTypes::FP32) //
      FILTER_SET_ENUM_VALUE(64, DataTypes::FP64) //
    FILTER_SET_ENUM_END("valid values are 32 and 64") //
    FILTER_SET_END()
}

Filter::ParamChangeStatus
ViewInput::setViewParameter(const std::string& key, const ViewWrapper& value) {
  FILTER_SET(view, false)
  FILTER_SET_END()
}

ViewInput::Job::Job(std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output)
    : FilterJob(std::move(input), std::move(output)) {}

template<class FloatType, class ComplexType>
struct Functor {
  void operator()(Buffer* output, const ViewInput::Params& params) {
    auto& output_buf = dynamic_cast<ViewBuffer<FloatType>&>(*output);
    output_buf.view = (RenderingView<FloatType>) params.view;
  }
};

void ViewInput::Job::complete() {
  Buffer::callWithPrecision<Functor>(output.get(), output.get(), params);
  output->notifyCompletion();
}
