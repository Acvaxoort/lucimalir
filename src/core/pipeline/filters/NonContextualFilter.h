//
// Created by rutio on 2020-05-29.
//

#ifndef LUCIMALIR_NONCONTEXTUALFILTER_H
#define LUCIMALIR_NONCONTEXTUALFILTER_H

#include <core/pipeline/Filter.h>
#include "NonContextualFilterJob.h"

template<class InputType, class OutputType>
class NonContextualFilter : public Filter {
public:
  int width = 1920;
  int height = 1080;
  int num_layers = 1;

protected:
  virtual NonContextualFilterJob<InputType, OutputType>*
  makeNextJobNonContextual(
      std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output) = 0;

public:
  double getProgress() const override {
    if (num_segments == 0) {
      return 0.0;
    }
    return (next_segment - (getJobCount() * 0.5)) / num_segments;
  }

  OptionalError canOutputTo(Buffer* output) const final {
    return Filter::verifyMatrixBuffer<OutputType>(output);
  }

  OptionalError adaptToOutput(Buffer* output, FilterWrapper* context) final {
    auto& output_buf = dynamic_cast<MatrixArrayBuffer<OutputType>&>(*output);
    if (width != output_buf.getWidth() || height != output_buf.getHeight() ||
        num_layers != output_buf.getNumLayers()) {
      width = (int) output_buf.getWidth();
      height = (int) output_buf.getHeight();
      num_layers = (int) output_buf.getNumLayers();
      return context->tryChangeInputNoLock(
          Buffer::makeSharedBuffer<MatrixArrayBuffer<InputType>>(
              num_layers, width, height));
    } else {
      return true;
    }
  }

  OptionalError canInputFrom(Buffer* input) const final {
    auto status = Filter::verifyMatrixBuffer<InputType>(input);
    if (status.success) {
      auto& input_buf = dynamic_cast<MatrixArrayBuffer<InputType>&>(*input);
      if (input_buf.getWidth() != width || input_buf.getHeight() != height ||
          input_buf.getNumLayers() != num_layers) {
        std::stringstream ss;
        ss << "buffer dimensions " << input_buf.getWidth() << "x"
           << input_buf.getHeight()
           << "[" << input_buf.getNumLayers()
           << "] don't match current output dimensions: "
           << width << "x" << height << "[" << num_layers << "]";
        return ss.str();
      }
    }
    return status;
  }

  OptionalError adaptToInput(Buffer* input) final {
    return true;
  }

  std::shared_ptr<Buffer> makeInputBuffer() const final {
    return Buffer::makeSharedBuffer<MatrixArrayBuffer<InputType>>(num_layers,
                                                                  width,
                                                                  height);
  }

  int next_segment = 0;
  int num_segments = 0;

protected:
  FilterJob* makeNextJob(
      std::shared_ptr<Buffer> input,
      std::shared_ptr<Buffer> output) final {
    if (input->getNumCompletedSegments() > next_segment) {
      auto ret = makeNextJobNonContextual(std::move(input), std::move(output));
      ret->segment_index = next_segment;
      next_segment++;
      return ret;
    }
    return nullptr;
  }
};


#endif //LUCIMALIR_NONCONTEXTUALFILTER_H
