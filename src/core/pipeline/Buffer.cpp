//
// Created by rutio on 2020-04-06.
//

#include <core/Logger.h>
#include "Buffer.h"
#include "BufferCollection.h"

std::shared_ptr<Buffer> Buffer::getManagedThisPointer() {
  return this_managed_pointer.lock();
}

bool Buffer::isCompleted() const {
  auto lock = local_mutex.getLock();
  return num_segments == computed_segments;
}

int Buffer::getNumCompletedSegments() const {
  auto lock = local_mutex.getLock();
  return computed_segments;
}

int Buffer::getNumSegments() const {
  //auto lock = local_mutex.getLock();
  return num_segments;
}

void Buffer::resetCompletion() {
  auto lock = local_mutex.getLock();
  computed_segments = 0;
  stray_computed_segments.clear();
}

void Buffer::notifyCompletion() {
  SharedMutexWrapper::Lock lock0;
  if (owner) {
    lock0 = owner->shared_mutex.getLock();
  }
  auto lock = local_mutex.getLock();
  computed_segments = num_segments;
  if (owner) {
    owner->notifySegmentCompletionNoLock(this);
    owner->notifyCompletionNoLock(this);
  }
}

void Buffer::notifySegmentCompletion(int64_t index) {
  SharedMutexWrapper::Lock lock0;
  if (owner) {
    lock0 = owner->shared_mutex.getLock();
  }
  auto lock = local_mutex.getLock();
  if (index == computed_segments) {
    computed_segments++;
    while (stray_computed_segments.count(computed_segments)) {
      stray_computed_segments.erase(computed_segments);
      computed_segments++;
    }
  } else {
    stray_computed_segments.insert(index);
  }
  if (owner) {
    owner->notifySegmentCompletionNoLock(this);
    if (computed_segments == num_segments) {
      owner->notifyCompletionNoLock(this);
    }
  }
}

BufferCollection* Buffer::getOwner() {
  auto lock = local_mutex.getLock();
  return owner;
}

void Buffer::setOwner(BufferCollection* new_owner) {
  auto lock = local_mutex.getLock();
  owner = new_owner;
}

std::optional<DataTypes::FloatingPrecision> Buffer::getPrecision(Buffer* buf) {
  if (dynamic_cast<ViewBuffer<DataTypes::FLOAT32>*>(buf)) {
    return DataTypes::FP32;
  } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::FLOAT32>*>(buf)) {
    return DataTypes::FP32;
  } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::QUATERNION32>*>(buf)) {
    return DataTypes::FP32;
  } else if (dynamic_cast<ViewBuffer<DataTypes::FLOAT64>*>(buf)) {
    return DataTypes::FP64;
  } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::FLOAT64>*>(buf)) {
    return DataTypes::FP64;
  } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::QUATERNION64>*>(buf)) {
    return DataTypes::FP64;
  } else if (dynamic_cast<ViewBuffer<DataTypes::FLOAT128>*>(buf)) {
    return DataTypes::FP128;
  } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::FLOAT128>*>(buf)) {
    return DataTypes::FP128;
  } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::QUATERNION128>*>(buf)) {
    return DataTypes::FP128;
  } else {
    return {};
  }
}
