//
// Created by rutio on 2020-04-11.
//

#ifndef LUCIMALIR_CORE_H
#define LUCIMALIR_CORE_H


#include <vector>
#include <set>
#include <thread>
#include <core/pipeline/FilterWrapper.h>
#include <core/wrappers/WeakPtrFilterWrapper.h>
#include <core/util/ThreadBlocker.h>

class Core {
public:
  static Core& getInstance() {
    static Core instance;
    return instance;
  }

  // Managing the threads
  int getNumThreads();

  void setNumThreads(int num);

  // Waits until all computing ends
  void awaitCompletion();

  // If any thread is working
  bool isWorking();

  // End all threads
  void shutDown();

  // Managing the filters
  WeakPtrFilterWrapper addFilter(Filter* filter);

  WeakPtrFilterWrapper addFilterWrapper(FilterWrapper* filter);

  void removeFilter(const WeakPtrFilterWrapper& filter);

  // Update the collections of filters with a particular flag
  void updateFilterGroups();

  // Set priority for a pipeline, threads are distributed proportionally to the priority, higher = more threads
  OptionalError
  setPriority(const WeakPtrFilterWrapper& filter, double priority);

  //& Get priority for a pipeline, if filter is invalid, returns -1
  double getPriority(const WeakPtrFilterWrapper& filter);

  // Getters for the fil&ters with a particular flag
  std::vector<WeakPtrFilterWrapper> getFilters() const;

  std::vector<WeakPtrFilterWrapper> getExternalInputFilters() const;

  std::vector<WeakPtrFilterWrapper> getExternalOutputFilters() const;

  std::vector<WeakPtrFilterWrapper> getFilterGroupRoots() const;

  std::vector<WeakPtrFilterWrapper> getPipelineRoots() const;

  std::vector<WeakPtrFilterWrapper> getStrayFilters() const;

private:
  class WorkerThread {
  public:
    explicit WorkerThread(Core* core = nullptr);

    void run();

    void notifyActivity();

    void notifyInactivity();

    bool interrupted = false;
    bool running = true;

    std::thread thread;
    Core* core;
  };

  struct PipelineData {
    int thread_count = 0;
    double priority = 1;
  };

  struct PipelineWithScore {
    std::weak_ptr<FilterWrapper> root;
    double score;
  };

  std::vector<PipelineWithScore> getPipelines();

  void incrementCounter(const std::weak_ptr<FilterWrapper>& filter);

  void decrementCounter(const std::weak_ptr<FilterWrapper>& filter);

  std::set<std::shared_ptr<FilterWrapper>> filters;

  // Remembering which filters have some flags
  std::vector<WeakPtrFilterWrapper> external_input_filters;
  std::vector<WeakPtrFilterWrapper> external_output_filters;
  std::vector<WeakPtrFilterWrapper> filter_group_roots;
  std::vector<WeakPtrFilterWrapper> pipeline_roots;
  std::vector<WeakPtrFilterWrapper> stray_filters;

  std::map<std::weak_ptr<FilterWrapper>, PipelineData, std::owner_less<std::weak_ptr<FilterWrapper>>> pipelines_data;

  std::list<WorkerThread> worker_threads;

  mutable std::mutex main_mutex;

  mutable std::mutex completion_notif_mutex;

  int num_active_filters = 0;

  ThreadBlocker inactive_notification_blocker;

  Core();
};


#endif //LUCIMALIR_CORE_H
