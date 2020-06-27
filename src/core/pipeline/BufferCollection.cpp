//
// Created by rutio on 2020-04-06.
//

#include "BufferCollection.h"
#include "FilterWrapper.h"
#include "Filter.h"
#include <core/GlobalConfig.h>
#include <core/Logger.h>

BufferCollection::BufferCollection(std::shared_ptr<Buffer> buffer) {
  buffer->setOwner(this);
  buffers.push_front(std::move(buffer));
  max_buffer_history = GlobalConfig::getInstance().default_buffer_history;
}

OptionalError BufferCollection::shiftBufferQueue(int shift) {
  auto lock0 = shared_mutex.getLock();
  auto lock = local_mutex.getLock();
  shift = ((shift % buffers.size()) + buffers.size()) % buffers.size();
  if (shift == 0) {
    return true;
  }
  OptionalError ret;
  if (!buffers.front()->isCompleted()) {
    for (FilterWrapper* f : inputting_filters) {
      f->interruptNoLock();
    }
    buffers.front()->resetCompletion();
    buffers.push_back(std::move(buffers.front()));
    buffers.pop_front();
  }
  auto new_begin = buffers.begin();
  for (int i = 0; i < shift; ++i) {
    ++new_begin;
  }
  Buffer* new_top_buffer = new_begin->get();
  if (!new_top_buffer->isTheSameType(buffers.front().get())) {
    for (FilterWrapper* f : inputting_filters) {
      auto status = f->filter->canInputFrom(new_top_buffer);
      if (!status.success) {
        return status.prependIfError([this, f]() {
          std::string ret;
          ret.reserve(80);
          return ret + "Filter " + f->filter->getName()
                 + " cannot input from the new top bufffer: ";
        });
      }
    }
    if (outputting_filter) {
      auto status = outputting_filter->filter->canOutputTo(new_top_buffer);
      if (!status.success) {
        return status.prependIfError([this]() {
          std::string ret;
          ret.reserve(80);
          return ret + "Filter " + outputting_filter->filter->getName()
                 + " cannot output to the new top buffer: ";
        });
      }
      status = outputting_filter->filter->adaptToOutput(
          new_top_buffer, outputting_filter);
      ret.accumulate(std::move(status));
    }
    for (FilterWrapper* f : inputting_filters) {
      auto status = f->filter->adaptToInput(new_top_buffer);
      ret.accumulate(std::move(status));
    }
  }
  buffers.splice(buffers.end(), buffers, buffers.begin(), new_begin);
  return ret.prependIfError("Error while adapting to changes: ");
}

std::shared_ptr<Buffer> BufferCollection::getTopBuffer() const {
  auto lock = local_mutex.getLock();
  return buffers.front();
}

std::shared_ptr<Buffer> BufferCollection::getBuffer(int index) const {
  auto lock = local_mutex.getLock();
  if (index < 0 || index >= buffers.size()) {
    return nullptr;
  } else {
    auto it = buffers.begin();
    for (int i = 0; i < index; ++i) {
      ++it;
    }
    return *it;
  }
}

std::list<std::shared_ptr<Buffer>> BufferCollection::getBuffers() const {
  auto lock = local_mutex.getLock();
  return buffers;
}

FilterWrapper* BufferCollection::getNextInputtingFilter() {
  auto lock = local_mutex.getLock();
  int num_inputting_filters = inputting_filters.size();
  if (num_inputting_filters == 0) {
    return nullptr;
  }
  FilterWrapper* result;
  int counter = 0;
  do {
    next_chosen_inputting_filter %= inputting_filters.size();
    result = inputting_filters[next_chosen_inputting_filter];
    next_chosen_inputting_filter++;
    counter++;
    if (counter > num_inputting_filters) {
      result = nullptr;
      break;
    }
  } while (!result->isConnectedToExternalOutput());
  return result;
}

uint32_t BufferCollection::getMaxBufferHistory() {
  return max_buffer_history;
}

void BufferCollection::setMaxBufferHistory(uint32_t value) {
  max_buffer_history = value;
}

void BufferCollection::cycleBuffersNoLock() {
  if (buffers.front()->isCompleted()) {
    if (buffers.size() > max_buffer_history) {
      if (buffers.front()->isTheSameType(buffers.back().get())) {
        buffers.back()->resetCompletion();
        buffers.push_front(std::move(buffers.back()));
        buffers.pop_back();
      } else {
        auto new_buf = buffers.front()->makeNewOfSameType();
        new_buf->setOwner(this);
        buffers.push_front(new_buf);
        buffers.back()->setOwner(nullptr);
        buffers.pop_back();
      }
    } else {
      auto new_buf = buffers.front()->makeNewOfSameType();
      new_buf->setOwner(this);
      buffers.push_front(new_buf);
    }
  } else {
    for (FilterWrapper* f : inputting_filters) {
      f->interruptNoLock();
    }
    buffers.front()->resetCompletion();
  }
}

OptionalError BufferCollection::tryReplaceBufferNoLock(
    std::shared_ptr<Buffer> buffer, FilterWrapper* source) {
  if (buffer->getOwner()) {
    return "The buffer that is being added already belongs to a collection";
  }
  OptionalError ret;
  if (!buffer->isTheSameType(buffers.front().get())) {
    for (FilterWrapper* f : inputting_filters) {
      if (f == source) {
        continue;
      }
      auto status = f->filter->canInputFrom(buffer.get());
      if (!status.success) {
        return status.prependIfError([f]() {
          std::string ret;
          ret.reserve(80);
          return ret + "Filter " + f->filter->getName()
                 + " cannot input from the new bufffer: ";
        });
      }
    }
    if (outputting_filter) {
      auto status = outputting_filter->filter->canOutputTo(buffer.get());
      if (!status.success) {
        return status.prependIfError([this]() {
          std::string ret;
          ret.reserve(80);
          return ret + "Filter " + outputting_filter->filter->getName()
                 + " cannot output to the new buffer: ";
        });
      }
      status = outputting_filter->filter->adaptToOutput(
          buffer.get(), outputting_filter);
      if (ret.success) {
        ret = std::move(status);
      } else {
        ret.message += "; " + status.message;
      }
    }
    for (FilterWrapper* f : inputting_filters) {
      if (f != source) {
        auto status = f->filter->adaptToInput(buffer.get());
        ret.accumulate(std::move(status));
      }
    }
  }
  for (FilterWrapper* f : inputting_filters) {
    if (f != source) {
      f->interruptNoLock();
    }
  }
  buffers.front()->resetCompletion();
  if (buffers.size() > max_buffer_history) {
    buffers.back()->setOwner(nullptr);
    buffers.pop_back();
  }
  buffer->setOwner(this);
  buffers.push_front(std::move(buffer));
  return ret.prependIfError("Error while adapting to changes: ");
}

FilterWrapper* BufferCollection::getOutputtingFilterNoLock() const {
  return outputting_filter;
}

void BufferCollection::setOutputtingFilterNoLock(FilterWrapper* filter) {
  outputting_filter = filter;
}

std::vector<FilterWrapper*>
BufferCollection::getInputtingFiltersNoLock() const {
  return inputting_filters;
}

void BufferCollection::addInputtingFilterNoLock(FilterWrapper* filter) {
  auto it = std::find(inputting_filters.begin(), inputting_filters.end(),
                      filter);
  if (it == inputting_filters.end()) {
    inputting_filters.emplace_back(filter);
    if (buffers.front()->isCompleted()) {
      filter->requestRenderingNoLock();
    }
  }
}

void BufferCollection::removeInputtingFilterNoLock(FilterWrapper* filter) {
  auto it = std::find(inputting_filters.begin(), inputting_filters.end(),
                      filter);
  if (it != inputting_filters.end()) {
    if (next_chosen_inputting_filter
        > std::distance(it, inputting_filters.begin())) {
      next_chosen_inputting_filter--;
    }
    inputting_filters.erase(it);
  }
}

void BufferCollection::notifyCompletionNoLock(Buffer* source) {
  if (source == buffers.front().get()) {
    if (outputting_filter) {
      outputting_filter->completeRenderingNoLock();
    }
  }
}

void BufferCollection::notifySegmentCompletionNoLock(Buffer* source) {
  if (source == buffers.front().get()) {
    for (auto f : inputting_filters) {
      f->requestRenderingNoLock(
          outputting_filter ? outputting_filter->working_filter->unique_id
                            : -1);
    }
  }
}

void BufferCollection::propagateSharedMutexNoLock(
    const SharedMutexWrapper& new_shared_mutex) {
  shared_mutex = new_shared_mutex;
  for (auto* f : inputting_filters) {
    f->propagateSharedMutexNoLock(new_shared_mutex);
  }
}

void BufferCollection::updatePipelineSharedMutexNoLock() {
  if (outputting_filter) {
    outputting_filter->updatePipelineSharedMutexNoLock();
  }
}
