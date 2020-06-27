//
// Created by rutio on 2020-05-30.
//

#ifndef LUCIMALIR_THREADBLOCKER_H
#define LUCIMALIR_THREADBLOCKER_H

#include <mutex>
#include <condition_variable>

class ThreadBlocker {
public:
  void reset();
  void wait();
  void notify();

private:
  std::mutex mut;
  std::condition_variable cv;
  bool ready = false;
};


#endif //LUCIMALIR_THREADBLOCKER_H
