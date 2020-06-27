//
// Created by rutio on 2020-04-09.
//

#include "Filter.h"

void Filter::waitTillCompletion() {

}

int Filter::getJobCount() const {
  //std::lock_guard<std::mutex> lock(job_list_mutex);
  auto lock = local_mutex.getLock();
  return jobs.size();
}

void Filter::interrupt() {
  //std::lock_guard<std::mutex> lock(job_list_mutex);
  auto lock = local_mutex.getLock();
  interruptNoLock();
}

void Filter::interruptNoLock() {
  interrupted = true;
  for (auto& j : jobs) {
    auto sj = j.lock();
    if (sj) {
      sj->interrupted = true;
    }
  }
}

void Filter::unregisterJob(FilterJob* job) {
  //std::lock_guard<std::mutex> lock(job_list_mutex);
  auto lock = local_mutex.getLock();
  auto it = std::find_if(
      jobs.begin(), jobs.end(),
      [job](auto& p) {
        return p.lock().get() == job;
      });
  jobs.erase(it);
}

std::shared_ptr<FilterJob> Filter::getNextJob(
    std::shared_ptr<Buffer> input,
    std::shared_ptr<Buffer> output) {
  //std::lock_guard<std::mutex> lock(job_list_mutex);
  auto lock = local_mutex.getLock();
  return getNextJobNoLock(std::move(input), std::move(output));
}

std::shared_ptr<FilterJob> Filter::getNextJobNoLock(
    std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output) {
  if (jobs.empty() || !interrupted) {
    interrupted = false;
    auto ptr = makeNextJob(std::move(input), std::move(output));
    if (ptr) {
      auto ret = std::shared_ptr<FilterJob>(ptr);
      ptr->origin = this_weak_ptr;
      jobs.emplace_back(ret);
      return ret;
    }
  }
  return nullptr;
}

void Filter::disconnectJobs() {
  std::list<std::shared_ptr<FilterJob>> jobs_temp;
  std::list<std::unique_lock<std::mutex>> jobs_locks;
  // forced mutex order to avoid deadlocking with FilterJob destructor
  {
    auto lock = local_mutex.getLock();
    interrupted = true;
    interruptNoLock();
    for (auto& j : jobs) {
      auto sj = j.lock();
      if (sj) {
        jobs_locks.emplace_back(sj->local_mutex.getDeferLock());
        sj->interrupted = true;
        jobs_temp.emplace_back(std::move(sj));
      }
    }
  }
  for (auto& jl : jobs_locks) {
    jl.lock();
  }
  {
    auto lock = local_mutex.getLock();
    for (auto& j : jobs_temp) {
      j->origin.reset();
    }
  }
}

Filter::Filter(const Filter& other) : context(other.context) {

}

void Filter::registerFilterId() {
  static std::mutex mut;
  std::lock_guard<std::mutex> lock(mut);
  static int next_id = 0;
  unique_id = next_id;
  next_id++;
}

void Filter::registerNegativeFilterId() {
  static std::mutex mut;
  std::lock_guard<std::mutex> lock(mut);
  static int next_id = -2;
  unique_id = next_id;
  next_id--;
}

bool Filter::hasNumberParameter(const std::string& key) {
  return false;
}

bool Filter::hasStringParameter(const std::string& key) {
  return false;
}

bool Filter::hasViewParameter(const std::string& key) {
  return false;
}

bool Filter::hasParameter(const std::string& key) {
  return false;
}

double Filter::getNumberParameter(const std::string& key) {
  return 0.0;
}

std::string Filter::getStringParameter(const std::string& key) {
  return "";
}

ViewWrapper Filter::getViewParameter(const std::string& key) {
  return {};
}

SharedPtrParameter Filter::getParameter(const std::string& key) {
  return nullptr;
}

Filter::ParamChangeStatus Filter::setNumberParameter(
    const std::string& key, double value) {
  return {};
}

Filter::ParamChangeStatus Filter::setStringParameter(
    const std::string& key, const std::string& value) {
  return {};
}

Filter::ParamChangeStatus
Filter::setViewParameter(const std::string& key, const ViewWrapper& value) {
  return {};
}

Filter::ParamChangeStatus Filter::setParameter(
    const std::string& key, SharedPtrParameter value) {
  return {};
}