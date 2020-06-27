//
// Created by rutio on 2020-06-02.
//

#include "PartitionedProcessingTracker.h"

PartitionedProcessingTracker::PartitionedProcessingTracker(int count)
    : processed(count), amount(count) {}

int PartitionedProcessingTracker::getNumDispatched() {
  std::lock_guard<std::mutex> lock(mut);
  return dispatched;
}

int PartitionedProcessingTracker::getNumTotal() {
  std::lock_guard<std::mutex> lock(mut);
  return amount;
}

bool PartitionedProcessingTracker::didDispatchAll() {
  std::lock_guard<std::mutex> lock(mut);
  return dispatched == amount;
}

bool PartitionedProcessingTracker::didProcessAll() {
  std::lock_guard<std::mutex> lock(mut);
  for (bool b : processed) {
    if (!b) {
      return false;
    }
  }
  return true;
}

int PartitionedProcessingTracker::getNextIndex() {
  std::lock_guard<std::mutex> lock(mut);
  if (dispatched < amount) {
    int x = dispatched;
    dispatched++;
    return x;
  } else {
    return -1;
  }
}

void PartitionedProcessingTracker::notifyProcessingEnd(int index) {
  std::lock_guard<std::mutex> lock(mut);
  processed[index] = true;
}
