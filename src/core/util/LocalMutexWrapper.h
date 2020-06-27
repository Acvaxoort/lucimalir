//
// Created by rutio on 2020-06-06.
//

#ifndef LUCIMALIR_LOCALMUTEXWRAPPER_H
#define LUCIMALIR_LOCALMUTEXWRAPPER_H

#include <mutex>

class LocalMutexWrapper {
public:
  LocalMutexWrapper() = default;

  LocalMutexWrapper(const LocalMutexWrapper& other);

  LocalMutexWrapper& operator=(const LocalMutexWrapper& other);

  LocalMutexWrapper(LocalMutexWrapper&& other) noexcept;

  LocalMutexWrapper& operator=(LocalMutexWrapper&& other) noexcept;

  std::lock_guard<std::mutex> getLock() const;

  std::unique_lock<std::mutex> getDeferLock() const;

private:
  mutable std::mutex mutex;
};


#endif //LUCIMALIR_LOCALMUTEXWRAPPER_H
