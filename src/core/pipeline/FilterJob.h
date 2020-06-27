//
// Created by rutio on 2020-04-06.
//

#ifndef LUCIMALIR_FILTERJOB_H
#define LUCIMALIR_FILTERJOB_H

#include <memory>
#include "BufferCollection.h"

class Filter;

class FilterJob {
public:
  FilterJob(std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output);

  // Do the most of the work, will be called once for each job
  // Can call notifySegmentCompletion and notifyCompletion on buffers but only if isn't interrupted
  virtual void complete() = 0;

  // Do any optional finalizing work, will be called after complete if
  // the thread won't do another job from the same filter
  // Can call notifySegmentCompletion and notifyCompletion on buffers but only if isn't interrupted
  virtual void finalize();

  // Pass some results from previous job run on the same filter on the same thread,
  // will be called if the thread gets another job from the same filter
  virtual void mergeWithPrevious(std::shared_ptr<FilterJob> prev);

  // If this flag is true, complete should stop computing as soon as possible
  // and notifySegmentCompletion and notifyCompletion on buffers shouldn't be called
  bool interrupted = false;

  std::shared_ptr<FilterJob> getSuccessorJob();

  virtual ~FilterJob();

  LocalMutexWrapper local_mutex;
  //std::mutex mut;

protected:
  const std::shared_ptr<Buffer> input;
  const std::shared_ptr<Buffer> output;

  // Managed by the origin
  std::weak_ptr<Filter> origin;

  friend class Filter;
};

#endif //LUCIMALIR_FILTERJOB_H
