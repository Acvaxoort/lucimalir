//
// Created by rutio on 2020-05-29.
//

#ifndef LUCIMALIR_NONCONTEXTUALMERGEFILTER_H
#define LUCIMALIR_NONCONTEXTUALMERGEFILTER_H

#include <core/pipeline/Filter.h>
#include "NonContextualMergeFilterJob.h"

template<class InputType, class OutputType>
class NonContextualMergeFilter : public Filter {
public:
  int width = 1920;
  int height = 1080;
  int num_layers = 1;

protected:
  virtual NonContextualMergeFilterJob<InputType, OutputType>*
  makeNextJobNonContextualMerge(
      std::shared_ptr<Buffer> input,
      std::shared_ptr<Buffer> output) = 0;

public:
  double getProgress() const override {
    if (num_segments == 0) {
      return 0.0;
    }
    return (next_segment - (getJobCount() * 0.5)) / num_segments;
  }

  OptionalError canOutputTo(Buffer* output) const final {
    auto status = Filter::verifyMatrixBuffer<OutputType>(output);
    if (status.success) {
      auto p = dynamic_cast<MatrixArrayBuffer<OutputType>*>(output);
      if (p->getNumLayers() != 1) {
        return "expected buffer with one layer";
      }
    }
    return status;
  }

  OptionalError adaptToOutput(Buffer* output, FilterWrapper* context) final {
    auto& output_buf = dynamic_cast<MatrixArrayBuffer<OutputType>&>(*output);
    if (width != output_buf.getWidth() || height != output_buf.getHeight()) {
      width = output_buf.getWidth();
      height = output_buf.getHeight();
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
      if (input_buf.getWidth() != width || input_buf.getHeight() != height) {
        std::stringstream ss;
        ss << "buffer dimensions " << input_buf.getWidth() << "x"
           << input_buf.getHeight()
           << "don't match current output dimensions: " << width << "x"
           << height;
        return ss.str();
      }
    }
    return status;
  }

  OptionalError adaptToInput(Buffer* input) final {
    auto& input_buf = dynamic_cast<MatrixArrayBuffer<InputType>&>(*input);
    num_layers = (int) input_buf.getNumLayers();
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
      auto ret = makeNextJobNonContextualMerge(std::move(input),
                                               std::move(output));
      ret->segment_index = next_segment;
      next_segment++;
      return ret;
    }
    return nullptr;
  }
};


#endif //LUCIMALIR_NONCONTEXTUALMERGEFILTER_H
