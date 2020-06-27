//
// Created by aleksander on 05.06.20.
//

#ifndef LUCIMALIR_SHAREDMUTEXWRAPPER_H
#define LUCIMALIR_SHAREDMUTEXWRAPPER_H

#include <mutex>
#include <memory>
#include <optional>

class SharedMutexWrapper {
public:
  struct Lock {
    std::optional<std::unique_lock<std::mutex>> lock;
    std::shared_ptr<std::mutex> prevent_destruction_before_unlocking;
  };

  SharedMutexWrapper() = default;

  SharedMutexWrapper(const SharedMutexWrapper& other);

  SharedMutexWrapper& operator=(const SharedMutexWrapper& other);

  SharedMutexWrapper(SharedMutexWrapper&& other) noexcept;

  SharedMutexWrapper& operator=(SharedMutexWrapper&& other) noexcept;

  SharedMutexWrapper(const std::shared_ptr<std::mutex>& new_mutex);

  SharedMutexWrapper& operator=(const std::shared_ptr<std::mutex>& new_mutex);

  SharedMutexWrapper(std::shared_ptr<std::mutex>&& new_mutex) noexcept;

  SharedMutexWrapper& operator=(std::shared_ptr<std::mutex>&& new_mutex) noexcept;

  bool operator==(const SharedMutexWrapper& other) const;

  bool operator!=(const SharedMutexWrapper& other) const;

  void createNewMutex();

  void removeMutex();

  void updateMutex(std::shared_ptr<std::mutex> new_mutex);

  bool empty();

  Lock getLock();

private:
  std::shared_ptr<std::mutex> mutex;
  mutable std::mutex update_mutex;
};


#endif //LUCIMALIR_SHAREDMUTEXWRAPPER_H
