//
// Created by rutio on 2020-04-09.
//

#include "FilterJob.h"
#include "Filter.h"

FilterJob::FilterJob(
    std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output)
    : input(std::move(input)),
      output(std::move(output)) {
}

FilterJob::~FilterJob() {
  auto lock = local_mutex.getLock();
  auto origin_shared = origin.lock();
  if (origin_shared) {
    origin_shared->unregisterJob(this);
  }
}

std::shared_ptr<FilterJob> FilterJob::getSuccessorJob() {
  //SharedMutexWrapper::Lock lock0;
  //std::lock_guard<std::mutex> lock(mut);
  auto lock = local_mutex.getLock();
  auto origin_shared = origin.lock();
  if (origin_shared) {
    return origin_shared->getNextJob(input, output);
  } else {
    return nullptr;
  }
}

void FilterJob::finalize() {

}

void FilterJob::mergeWithPrevious(std::shared_ptr<FilterJob> prev) {

}
