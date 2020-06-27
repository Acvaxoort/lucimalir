//
// Created by rutio on 2020-04-11.
//

#include "Core.h"
#include "pipeline/Filter.h"
#include "Logger.h"

WeakPtrFilterWrapper Core::addFilter(Filter* filter) {
  std::lock_guard<std::mutex> lock(main_mutex);
  auto shared = std::make_shared<FilterWrapper>(filter);
  std::weak_ptr<FilterWrapper> ret = shared;
  filters.insert(std::move(shared));
  updateFilterGroups();
  return ret;
}

WeakPtrFilterWrapper Core::addFilterWrapper(FilterWrapper* filter) {
  std::lock_guard<std::mutex> lock(main_mutex);
  auto shared = std::shared_ptr<FilterWrapper>(filter);
  std::weak_ptr<FilterWrapper> ret = shared;
  filters.insert(std::move(shared));
  updateFilterGroups();
  return ret;
}

void Core::removeFilter(const WeakPtrFilterWrapper& filter) {
  std::lock_guard<std::mutex> lock(main_mutex);
  auto p = filter.filter_wrapper.lock();
  auto it = filters.find(p);
  if (it != filters.end()) {
    p->disconnectInput();
    p->disconnectOutput();
    filters.erase(it);
  }
  updateFilterGroups();
}

void Core::updateFilterGroups() {
  external_input_filters.clear();
  external_output_filters.clear();
  filter_group_roots.clear();
  pipeline_roots.clear();
  stray_filters.clear();
  auto previous_pipelines_data = std::move(pipelines_data);
  for (auto& p : filters) {
    if (p->isAnExternalInput()) {
      external_input_filters.emplace_back(p);
    }
    if (p->isAnExternalOutput()) {
      external_output_filters.emplace_back(p);
    }
    if (p->isFilterGroupRoot()) {
      filter_group_roots.emplace_back(p);
    }
    if (p->isPipelineRoot()) {
      pipeline_roots.emplace_back(p);
      auto it = previous_pipelines_data.find(p);
      if (it != previous_pipelines_data.end()) {
        pipelines_data[std::weak_ptr<FilterWrapper>(
            p)] = previous_pipelines_data[std::weak_ptr<FilterWrapper>(p)];
      } else {
        pipelines_data[std::weak_ptr<FilterWrapper>(p)] = {};
      }
    }
    if (p->isStray()) {
      stray_filters.emplace_back(p);
    }
  }
}

OptionalError Core::setPriority(
    const WeakPtrFilterWrapper& filter,
    double priority) {
  std::lock_guard<std::mutex> lock(main_mutex);
  if (priority >= 0) {
    auto it = pipelines_data.find(filter.filter_wrapper);
    if (it != pipelines_data.end()) {
      it->second.priority = priority;
      return true;
    } else {
      return "invalid filter";
    }
  } else {
    return "priority should be a positive number";
  }
}

double Core::getPriority(const WeakPtrFilterWrapper& filter) {
  std::lock_guard<std::mutex> lock(main_mutex);
  auto it = pipelines_data.find(filter.filter_wrapper);
  if (it != pipelines_data.end()) {
    return it->second.priority;
  }
  return -1;
}

std::vector<WeakPtrFilterWrapper> Core::getFilters() const {
  std::lock_guard<std::mutex> lock(main_mutex);
  return {filters.begin(), filters.end()};
}

std::vector<WeakPtrFilterWrapper> Core::getExternalInputFilters() const {
  std::lock_guard<std::mutex> lock(main_mutex);
  return external_input_filters;
}

std::vector<WeakPtrFilterWrapper> Core::getExternalOutputFilters() const {
  std::lock_guard<std::mutex> lock(main_mutex);
  return external_output_filters;
}

std::vector<WeakPtrFilterWrapper> Core::getFilterGroupRoots() const {
  std::lock_guard<std::mutex> lock(main_mutex);
  return filter_group_roots;
}

std::vector<WeakPtrFilterWrapper> Core::getPipelineRoots() const {
  std::lock_guard<std::mutex> lock(main_mutex);
  return pipeline_roots;
}

std::vector<WeakPtrFilterWrapper> Core::getStrayFilters() const {
  std::lock_guard<std::mutex> lock(main_mutex);
  return stray_filters;
}

int Core::getNumThreads() {
  std::lock_guard<std::mutex> lock(main_mutex);
  auto new_end = std::remove_if(worker_threads.begin(), worker_threads.end(),
                                [](auto& th) { return !th.running; });
  worker_threads.resize(std::distance(worker_threads.begin(), new_end));
  int now_running = (int) std::count_if(worker_threads.begin(),
                                        worker_threads.end(),
                                        [](auto& th) { return !th.interrupted; });
  return now_running;
}

void Core::setNumThreads(int num) {
  std::lock_guard<std::mutex> lock(main_mutex);
  if (num <= 0) {
    num = std::thread::hardware_concurrency();
  }
  if (num >= 256) {
    num = 256;
  }

  auto new_end = std::remove_if(worker_threads.begin(), worker_threads.end(),
                                [](auto& th) { return !th.running; });
  worker_threads.resize(std::distance(worker_threads.begin(), new_end));
  int now_running = (int) std::count_if(worker_threads.begin(),
                                        worker_threads.end(),
                                        [](auto& th) { return !th.interrupted; });
  if (now_running < num) {
    for (int i = now_running; i < num; ++i) {
      worker_threads.emplace_back(this);
    }
  } else {
    auto it = worker_threads.end();
    it--;
    for (int i = num; i < now_running; ++i) {
      while (it->interrupted) {
        it--;
      }
      it->interrupted = true;
      it--;
    }
  }
}

void Core::awaitCompletion() {
  if (isWorking()) {
    {
      std::lock_guard<std::mutex> lock(completion_notif_mutex);
      inactive_notification_blocker.reset();
    }
    inactive_notification_blocker.wait();
  }
}

bool Core::isWorking() {
  return num_active_filters > 0;
}

void Core::shutDown() {
  for (auto& t : worker_threads) {
    t.interrupted = true;
  }
  for (auto& t : worker_threads) {
    t.thread.join();
  }
}

Core::Core() {
  setNumThreads(12);
}

std::vector<Core::PipelineWithScore> Core::getPipelines() {
  std::lock_guard<std::mutex> lock(main_mutex);
  //double score = 1e308;
  //std::weak_ptr<FilterWrapper> best;
  std::vector<PipelineWithScore> pipelines;
  for (auto& p : pipelines_data) {
    double p_score = p.second.thread_count / p.second.priority;
    PipelineWithScore pws;
    pws.root = p.first;
    pws.score = p_score;
    pipelines.emplace_back(std::move(pws));
    //if (p_score < score) {
    //  score = p_score;
    //  best = p.first;
    //}
  }
  std::sort(
      pipelines.begin(), pipelines.end(),
      [](auto& p1, auto& p2) {
        return p1.score < p2.score;
      });
  //if (!best.expired()) {
  //  pipelines_data[best].thread_count++;
  //}
  return pipelines;
}

void Core::incrementCounter(const std::weak_ptr<FilterWrapper>& filter) {
  std::lock_guard<std::mutex> lock(main_mutex);
  if (!filter.expired()) {
    auto it = pipelines_data.find(filter);
    if (it != pipelines_data.end()) {
      it->second.thread_count++;
    }
  }
}

void Core::decrementCounter(const std::weak_ptr<FilterWrapper>& filter) {
  std::lock_guard<std::mutex> lock(main_mutex);
  if (!filter.expired()) {
    auto it = pipelines_data.find(filter);
    if (it != pipelines_data.end()) {
      it->second.thread_count--;
    }
  }
}

void Core::WorkerThread::run() {
  logger::register_logger_thread_id();
  heavyDebugC("Entered thread function");
  const double SLEEP_PERIOD_MAX = 0.05;
  const double SLEEP_PERIOD_MIN = 0.001;
  const double SLEEP_PERIOD_MUL_INC = 1.2;
  const double SLEEP_PERIOD_MUL_DEC = 0.8;
  const int FAILS_UNTIL_SLEEP = 10;
  double sleep_period = SLEEP_PERIOD_MAX;
  int counter_until_sleep = 0;
  notifyActivity();
  while (!interrupted) {
    bool succesful = false;
    std::shared_ptr<FilterJob> current_job;
    std::weak_ptr<FilterWrapper> pipeline_root;
    //pipeline_root = core->getPipelines();
    auto pipelines = core->getPipelines();
    for (auto& pl : pipelines) {
      pipeline_root = pl.root;
      core->incrementCounter(pipeline_root);
      if (!pipeline_root.expired()) {
        FilterWrapper* filter = pipeline_root.lock().get();
        while (filter) {
          //heavyDebugC("Job search loop");
          current_job = filter->getJob();
          if (current_job) {
            //heavyDebugC("Got a job on %s",
            //            filter->getFilter().lock()->getName().c_str());
            break;
          } else {
            //heavyDebugC("Get next filter");
            filter = filter->getNextFilter();
          }
        }
        while (current_job) {
          succesful = true;
          current_job->complete();
          std::shared_ptr<FilterJob> new_job = current_job->getSuccessorJob();
          if (new_job) {
            //heavyDebugC("Completed job on %s, got another, merging",
            //            filter->getFilter().lock()->getName().c_str());
            new_job->mergeWithPrevious(std::move(current_job));
            current_job = std::move(new_job);
          } else {
            //heavyDebugC("Completed job on %s, finalizing",
            //            filter->getFilter().lock()->getName().c_str());
            current_job->finalize();
            current_job = nullptr;
          }
        }
        //heavyDebugC("No job available from the pipeline");
      } else {
        //heavyDebugC("No pipeline root");
      }
      core->decrementCounter(pipeline_root);
      if (succesful) {
        break;
      }
    }
    if (succesful) {
      counter_until_sleep = 0;
      sleep_period = std::max(sleep_period * SLEEP_PERIOD_MUL_DEC,
                              SLEEP_PERIOD_MIN);
    } else {
      counter_until_sleep++;
      if (counter_until_sleep == FAILS_UNTIL_SLEEP) {
        notifyInactivity();
        counter_until_sleep = 0;
        std::this_thread::sleep_for(
            std::chrono::milliseconds((int) (1000 * sleep_period)));
        sleep_period = std::min(sleep_period * SLEEP_PERIOD_MUL_INC,
                                SLEEP_PERIOD_MAX);
        notifyActivity();
      }
    }
  }
  notifyInactivity();
  heavyDebugC("Exiting thread function");
  running = false;
}

void Core::WorkerThread::notifyActivity() {
  std::lock_guard<std::mutex> lock(core->completion_notif_mutex);
  core->num_active_filters++;
}

void Core::WorkerThread::notifyInactivity() {
  std::lock_guard<std::mutex> lock(core->completion_notif_mutex);
  core->num_active_filters--;
  if (core->num_active_filters == 0) {
    core->inactive_notification_blocker.notify();
  }
}

Core::WorkerThread::WorkerThread(Core* core)
    : core(core) {
  thread = std::thread(&Core::WorkerThread::run, this);
}
