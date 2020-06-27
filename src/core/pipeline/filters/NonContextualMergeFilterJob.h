//
// Created by rutio on 2020-05-30.
//

#ifndef LUCIMALIR_NONCONTEXTUALMERGEFILTERJOB_H
#define LUCIMALIR_NONCONTEXTUALMERGEFILTERJOB_H

#include <core/pipeline/FilterJob.h>

template<class InputType, class OutputType>
class NonContextualMergeFilterJob : public FilterJob {
public:
  NonContextualMergeFilterJob(
      std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output)
      : FilterJob(std::move(input), std::move(output)) {}

  virtual void completeNonContextualMerge(
      MatrixLayerArrayView<InputType> input,
      MatrixLayerArrayView<OutputType> output) = 0;

  void complete() final {
    auto& input_array_buffer = dynamic_cast<MatrixArrayBuffer<InputType>&>(*input);
    auto& output_array_buffer = dynamic_cast<MatrixArrayBuffer<OutputType>&>(*output);
    completeNonContextualMerge(input_array_buffer.getSegmentView(segment_index),
                               output_array_buffer.getSegmentView(
                                   segment_index));
    output_array_buffer.notifySegmentCompletion(segment_index);
  }

  int segment_index;
};


#endif //LUCIMALIR_NONCONTEXTUALMERGEFILTERJOB_H
