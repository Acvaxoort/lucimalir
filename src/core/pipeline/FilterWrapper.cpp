//
// Created by rutio on 2020-04-06.
//

#include <core/Logger.h>
#include <core/GlobalConfig.h>
#include "FilterWrapper.h"
#include "BufferCollection.h"
#include "Filter.h"

/*
FilterWrapper::FilterWrapper(std::shared_ptr<Filter> filter)
    : filter(std::move(filter)),
      is_connected_to_external_output(this->filter->hasExternalOutput()),
      is_filter_group_root(false),
      is_stray(true),
      postponed_rendering_request(false) {
  this->filter->context = this;
  auto_request_on_changes = GlobalConfig::getInstance().filter_auto_request;
}*/

FilterWrapper::FilterWrapper(Filter* filter)
    : filter(filter),
      is_connected_to_external_output(this->filter->hasExternalOutput()),
      is_filter_group_root(false),
      is_stray(true),
      postponed_rendering_request(false) {
  this->filter->context = this;
  auto_request_on_changes = GlobalConfig::getInstance().filter_auto_request;
}

bool FilterWrapper::isCurrentlyRendering() const {
  auto lock = local_mutex.getLock();
  return working_filter.get();
}

bool FilterWrapper::isAnExternalInput() const {
  auto lock = local_mutex.getLock();
  return filter->hasExternalInput();
}

bool FilterWrapper::isAnExternalOutput() const {
  auto lock = local_mutex.getLock();
  return filter->hasExternalOutput();
}

bool FilterWrapper::isConnectedToExternalOutput() const {
  auto lock = local_mutex.getLock();
  return is_connected_to_external_output;
}

bool FilterWrapper::isFilterGroupRoot() const {
  return is_filter_group_root;
}

bool FilterWrapper::isPipelineRoot() const {
  return is_filter_group_root && is_connected_to_external_output;
}

bool FilterWrapper::isStray() const {
  return is_stray;
}

bool FilterWrapper::isAutoRequestOnChanges() const {
  return auto_request_on_changes;
}

bool FilterWrapper::hasFutureRenderingRequest() const {
  return future_rendering_request;
}

void FilterWrapper::setAutoRequestOnChanges(bool value) {
  auto_request_on_changes = value;
}

OptionalError FilterWrapper::canReplaceFilter(Filter* new_filter) const {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  if (output) {
    auto status = new_filter->canOutputTo(output->getTopBuffer().get());
    if (!status.success) {
      return status.prependIfError(
          "Filter cannot use the present output buffer: ");
    }
  }
  if (input) {
    auto status = new_filter->canInputFrom(input->getTopBuffer().get());
    if (!status.success) {
      return status.prependIfError(
          "Filter cannot use the present input buffer: ");
    }
  }
  return true;
}

OptionalError FilterWrapper::replaceFilter(std::shared_ptr<Filter> new_filter) {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  OptionalError ret;
  if (output) {
    auto status = new_filter->adaptToOutput(output->getTopBuffer().get(), this);
    ret.accumulate(std::move(status));
  }
  if (input) {
    auto status = new_filter->adaptToInput(input->getTopBuffer().get());
    ret.accumulate(std::move(status));
  }
  filter->context = nullptr;
  filter = std::move(new_filter);
  filter->context = this;
  updateConnectionStatusNoLock();
  interruptNoLock();
  if (auto_request_on_changes) {
    requestRenderingNoLock(-1);
  }
  return ret.prependIfError("Error while adapting to changes: ");
}

std::weak_ptr<Filter> FilterWrapper::getFilter() {
  auto lock = local_mutex.getLock();
  return filter;
}

Filter* FilterWrapper::getFilterRaw() const {
  auto lock = local_mutex.getLock();
  return filter.get();
}

std::weak_ptr<const Filter> FilterWrapper::getWorkingFilter() const {
  auto lock = local_mutex.getLock();
  return working_filter;
}

void FilterWrapper::interrupt() {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  interruptNoLock();
}

void FilterWrapper::requestRendering(int origin_id) {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  requestRenderingNoLock(origin_id);
}

OptionalError FilterWrapper::canConnectTo(FilterWrapper* prev) const {
  auto lock0 = shared_mutex.getLock();
  SharedMutexWrapper::Lock lockP0;
  if (shared_mutex != prev->shared_mutex) {
    lockP0 = prev->shared_mutex.getLock();
  }
  auto lock = local_mutex.getLock();
  auto lockP = prev->local_mutex.getLock();
  if (prev == this) {
    return "cannot connect to itself";
  }
  if (prev->output) {
    if (input) {
      return "one buffer already has an input and the other already has an "
             "output, disconnect one of the buffers";
    }
    return filter->canInputFrom(prev->output->getTopBuffer().get())
        .prependIfError([this, prev]() {
          std::string ret;
          ret.reserve(80);
          return ret + "Output filter " + filter->getName()
                 + " cannot input from " + prev->filter->getName() + ": ";
        });
  } else {
    if (input) {
      return prev->filter->canOutputTo(input->getTopBuffer().get())
          .prependIfError([this, prev]() {
            std::string ret;
            ret.reserve(80);
            return ret + "Input filter " + prev->filter->getName()
                   + " cannot output to " + filter->getName() + ": ";
          });
    } else {
      auto temp_buf = filter->makeInputBuffer();
      return prev->filter->canOutputTo(temp_buf.get())
          .prependIfError([this, prev]() {
            std::string ret;
            ret.reserve(80);
            return ret + "Input filter " + prev->filter->getName()
                   + " cannot output to " + filter->getName() + ": ";
          });
    }
  }
}

OptionalError FilterWrapper::connectTo(FilterWrapper* prev) {
  auto lock0 = shared_mutex.getLock();
  SharedMutexWrapper::Lock lockP0;
  if (shared_mutex != prev->shared_mutex) {
    lockP0 = prev->shared_mutex.getLock();
  }
  auto lock = local_mutex.getLock();
  auto lockP = prev->local_mutex.getLock();
  OptionalError ret;
  if (this == prev) {
    throw std::runtime_error("this == prev");
  }
  if (prev->output) {
    if (input) {
      throw std::runtime_error(
          "one buffer already has an input and the other already has an "
          "output, disconnect one of the buffers");
    }
    auto status = filter->adaptToInput(prev->output->getTopBuffer().get());
    ret.accumulate(std::move(status));
    input = prev->output;
    input->addInputtingFilterNoLock(this);
  } else {
    if (input) {
      auto status = prev->filter->adaptToOutput(
          input->getTopBuffer().get(), this);
      ret.accumulate(std::move(status));
      prev->output = input;
      input->setOutputtingFilterNoLock(prev);
    } else {
      auto temp_buf = filter->makeInputBuffer();
      input = std::make_shared<BufferCollection>(temp_buf);
      prev->output = input;
      input->addInputtingFilterNoLock(this);
      input->setOutputtingFilterNoLock(prev);
      auto status = prev->filter->adaptToOutput(temp_buf.get(), prev);
      ret.accumulate(std::move(status));
    }
  }
  if (prev->shared_mutex.empty()) {
    updatePipelineSharedMutexNoLock();
  } else {
    prev->propagateSharedMutexNoLock(prev->shared_mutex);
  }
  updateConnectionStatusNoLock();
  prev->updateConnectionStatusNoLock();
  propagateFutureRenderingRequestNoLock(prev->future_rendering_request);
  return ret.prependIfError("Error while adapting to changes: ");
}

void FilterWrapper::disconnectInput() {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  disconnectInputNoLock();
}

void FilterWrapper::disconnectOutput() {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  disconnectOutputNoLock();
}

std::weak_ptr<BufferCollection> FilterWrapper::getInputBuffer() {
  auto lock = local_mutex.getLock();
  return input;
}

std::weak_ptr<BufferCollection> FilterWrapper::getOutputBuffer() {
  auto lock = local_mutex.getLock();
  return output;
}

void FilterWrapper::notifyFilterInternalChanges() {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  if (auto_request_on_changes) {
    requestRenderingNoLock();
  }
}

OptionalError FilterWrapper::requestChangingInput() {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  return tryChangeInputNoLock(filter->makeInputBuffer());
}

FilterWrapper* FilterWrapper::getNextFilter() {
  auto lock = local_mutex.getLock();
  if (output) {
    return output->getNextInputtingFilter();
  } else {
    return nullptr;
  }
}

std::shared_ptr<FilterJob> FilterWrapper::getJob() {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  if (working_filter) {
    return working_filter->getNextJob(working_input, working_output);
  } else {
    return nullptr;
  }
}

void FilterWrapper::completeRendering() {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  completeRenderingNoLock();
}

OptionalError FilterWrapper::tryChangeInputNoLock(
    std::shared_ptr<Buffer> new_buffer) {
  if (!input) {
    //return "the filter isn't connected to an input buffer";
    return true;
  }
  auto status = input->tryReplaceBufferNoLock(std::move(new_buffer), this);
  if (!status.success) {
    disconnectInputNoLock();
  }
  return status;
}

void FilterWrapper::interruptNoLock() {
  if (working_filter) {
    working_filter->interrupt();
    working_filter->context = nullptr;
    working_filter->disconnectJobs();
  }
  working_filter = nullptr;
  working_input = nullptr;
  working_output = nullptr;
  if (postponed_rendering_request) {
    postponed_rendering_request = false;
    requestRenderingNoLock();
  } else {
    propagateFutureRenderingRequestNoLock(false);
  }
}

void FilterWrapper::requestRenderingNoLock(int origin_id) {
  bool external_input = isAnExternalInputNoLock();
  bool external_output = isAnExternalOutputNoLock();
  if ((input || external_input) && (output || external_output)) {
    if (!working_filter) {
      heavyDebugC("Honored a rendering request on %s by %d",
                  filter->getName().c_str(), origin_id);
      requesting_filter_id = origin_id;
      working_filter = filter->clone();
      working_filter->this_weak_ptr = working_filter;
      working_filter->registerNegativeFilterId();
      if (!external_input) {
        working_input = input->getTopBuffer();
      }
      if (!external_output) {
        output->cycleBuffersNoLock();
        working_output = output->getTopBuffer();
      }
      propagateFutureRenderingRequestNoLock(true);
      future_rendering_request = false;
    } else {
      if (origin_id != requesting_filter_id || origin_id == -1) {
        heavyDebugC("Postponed a rendering request on %s by %d",
                    filter->getName().c_str(), origin_id);
        postponed_rendering_request = true;
      } else {
        //heavyDebugC("Ignored a rendering request on %s by %d",
        //            filter->getName().c_str(), origin_id);
      }
    }
  }
}

void FilterWrapper::completeRenderingNoLock() {
  heavyDebugC("Finished %s", working_filter->getName().c_str());
  if (working_filter) {
    working_filter->context = nullptr;
    working_filter->disconnectJobs();
    working_filter = nullptr;
    working_input = nullptr;
    working_output = nullptr;
    if (postponed_rendering_request) {
      heavyDebugC("Honoring a postponed rendering request");
      postponed_rendering_request = false;
      requestRenderingNoLock();
    }
  }
}

void FilterWrapper::disconnectInputNoLock() {
  if (input) {
    propagateFutureRenderingRequestNoLock(false);
    input->removeInputtingFilterNoLock(this);
    if (auto f = input->getOutputtingFilterNoLock()) {
      f->updateConnectionStatusNoLock();
    }
    input = nullptr;
  }
}

void FilterWrapper::disconnectOutputNoLock() {
  if (output) {
    output->setOutputtingFilterNoLock(nullptr);
    for (auto f : output->getInputtingFiltersNoLock()) {
      f->updateConnectionStatusNoLock();
      f->propagateFutureRenderingRequestNoLock(false);
    }
    output = nullptr;
  }
}

void FilterWrapper::updateConnectionStatusNoLock() {
  bool previous_value = is_connected_to_external_output;
  if (filter->hasExternalOutput()) {
    is_connected_to_external_output = true;
  } else {
    is_connected_to_external_output = false;
    if (output) {
      for (auto f : output->getInputtingFiltersNoLock()) {
        if (f->is_connected_to_external_output) {
          is_connected_to_external_output = true;
          break;
        }
      }
    }
  }
  bool preceding_filter =
      input && input->getOutputtingFilterNoLock();
  bool succeeding_filter =
      output && !output->getInputtingFiltersNoLock().empty();
  if (previous_value != is_connected_to_external_output && preceding_filter) {
    input->getOutputtingFilterNoLock()->updateConnectionStatusNoLock();
  }
  is_stray = true;
  is_filter_group_root = false;
  if (succeeding_filter) {
    is_stray = false;
    is_filter_group_root = true;
  }
  if (preceding_filter) {
    is_stray = false;
    is_filter_group_root = false;
  }
}

void FilterWrapper::propagateSharedMutexNoLock(
    const SharedMutexWrapper& new_shared_mutex) {
  shared_mutex = new_shared_mutex;
  if (output) {
    output->propagateSharedMutexNoLock(new_shared_mutex);
  }
}

void FilterWrapper::updatePipelineSharedMutexNoLock() {
  if (filter->hasExternalInput()) {
    shared_mutex.createNewMutex();
    auto lock0 = shared_mutex.getLock();
    if (output) {
      output->propagateSharedMutexNoLock(shared_mutex);
    }
  } else {
    if (input) {
      input->updatePipelineSharedMutexNoLock();
    }
  }
}

void FilterWrapper::propagateFutureRenderingRequestNoLock(bool value) {
  future_rendering_request = value;
  if (output) {
    for (auto f : output->inputting_filters) {
      f->propagateFutureRenderingRequestNoLock(value);
    }
  }
}

bool FilterWrapper::isAnExternalInputNoLock() const {
  return filter->hasExternalInput();
}

bool FilterWrapper::isAnExternalOutputNoLock() const {
  return filter->hasExternalOutput();
}
