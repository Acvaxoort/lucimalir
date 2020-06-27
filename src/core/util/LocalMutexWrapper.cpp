//
// Created by rutio on 2020-06-06.
//

#include "LocalMutexWrapper.h"

LocalMutexWrapper::LocalMutexWrapper(const LocalMutexWrapper& other) {}

LocalMutexWrapper& LocalMutexWrapper::operator=(
    const LocalMutexWrapper& other) {
  return *this;
}

LocalMutexWrapper::LocalMutexWrapper(LocalMutexWrapper&& other) noexcept {}

LocalMutexWrapper& LocalMutexWrapper::operator=(
    LocalMutexWrapper&& other) noexcept {
  return *this;
}

std::lock_guard<std::mutex> LocalMutexWrapper::getLock() const {
  return std::lock_guard<std::mutex>(mutex);
}

std::unique_lock<std::mutex> LocalMutexWrapper::getDeferLock() const {
  return std::unique_lock<std::mutex>(mutex, std::defer_lock);
}
