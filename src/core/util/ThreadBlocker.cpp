//
// Created by rutio on 2020-05-30.
//

#include <core/Core.h>
#include "ThreadBlocker.h"

void ThreadBlocker::reset() {
  ready = false;
}

void ThreadBlocker::wait() {
  std::unique_lock<std::mutex> lock(mut);
  Core& core = Core::getInstance();
  while (!ready) {
    cv.wait(lock);
    //cv.wait_for(lock, std::chrono::milliseconds(1));
    //if (!core.isWorking()) {
    //  break;
    //}
  }
  ready = false;
}

void ThreadBlocker::notify() {
  std::unique_lock<std::mutex> lock(mut);
  ready = true;
  cv.notify_all();
}
