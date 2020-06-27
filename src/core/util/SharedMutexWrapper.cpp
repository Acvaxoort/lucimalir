//
// Created by aleksander on 05.06.20.
//

#include "SharedMutexWrapper.h"

SharedMutexWrapper::SharedMutexWrapper(const SharedMutexWrapper& other) {
  std::lock_guard<std::mutex> lock(other.update_mutex);
  mutex = other.mutex;
}

SharedMutexWrapper&
SharedMutexWrapper::operator=(const SharedMutexWrapper& other) {
  std::lock_guard<std::mutex> lock(other.update_mutex);
  mutex = other.mutex;
  return *this;
}

SharedMutexWrapper::SharedMutexWrapper(SharedMutexWrapper&& other) noexcept {
  std::lock_guard<std::mutex> lock(other.update_mutex);
  mutex = std::move(other.mutex);
}

SharedMutexWrapper&
SharedMutexWrapper::operator=(SharedMutexWrapper&& other) noexcept {
  std::lock_guard<std::mutex> lock(other.update_mutex);
  mutex = std::move(other.mutex);
  return *this;
}

SharedMutexWrapper::SharedMutexWrapper(
    const std::shared_ptr<std::mutex>& new_mutex)
    : mutex(new_mutex) {}

SharedMutexWrapper&
SharedMutexWrapper::operator=(const std::shared_ptr<std::mutex>& new_mutex) {
  std::lock_guard<std::mutex> lock(update_mutex);
  mutex = new_mutex;
  return *this;
}

SharedMutexWrapper::SharedMutexWrapper(
    std::shared_ptr<std::mutex>&& new_mutex) noexcept
    : mutex(new_mutex) {}

SharedMutexWrapper& SharedMutexWrapper::operator=(
    std::shared_ptr<std::mutex>&& new_mutex) noexcept {
  std::lock_guard<std::mutex> lock(update_mutex);
  mutex = new_mutex;
  return *this;
}

bool SharedMutexWrapper::operator==(const SharedMutexWrapper& other) const {
  std::lock_guard<std::mutex> lock(update_mutex);
  if (!mutex) {
    return false;
  }
  return mutex == other.mutex;
}

bool SharedMutexWrapper::operator!=(const SharedMutexWrapper& other) const {
  std::lock_guard<std::mutex> lock(update_mutex);
  if (!mutex) {
    return true;
  }
  return mutex != other.mutex;
}

void SharedMutexWrapper::createNewMutex() {
  std::lock_guard<std::mutex> lock(update_mutex);
  mutex = std::make_shared<std::mutex>();
}

void SharedMutexWrapper::removeMutex() {
  std::lock_guard<std::mutex> lock(update_mutex);
  mutex = nullptr;
}

void SharedMutexWrapper::updateMutex(std::shared_ptr<std::mutex> new_mutex) {
  std::lock_guard<std::mutex> lock(update_mutex);
  mutex = std::move(new_mutex);
}

bool SharedMutexWrapper::empty() {
  //std::lock_guard<std::mutex> lock(update_mutex);
  return mutex == nullptr;
}


SharedMutexWrapper::Lock SharedMutexWrapper::getLock() {
  std::lock_guard<std::mutex> lock(update_mutex);
  Lock ret;
  if (mutex) {
    ret.lock = std::unique_lock<std::mutex>(*mutex);
    ret.prevent_destruction_before_unlocking = mutex;
  }
  return ret;
}
