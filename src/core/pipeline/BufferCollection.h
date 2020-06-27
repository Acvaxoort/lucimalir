//
// Created by rutio on 2020-04-06.
//

#ifndef LUCIMALIR_BUFFERCOLLECTION_H
#define LUCIMALIR_BUFFERCOLLECTION_H

#include <list>
#include <core/util/LocalMutexWrapper.h>
#include <core/util/SharedMutexWrapper.h>
#include "Buffer.h"
#include "core/util/OptionalError.h"

class FilterWrapper;

class BufferCollection {
public:
  // -------------------------------------------------------------------------
  // Functions for external usage
  // -------------------------------------------------------------------------

  // Create the wrapper, buffer should not be null
  explicit BufferCollection(std::shared_ptr<Buffer> buffer);

  // Shift the buffer history queue by given amount of positions, 0 does
  // nothing -1 means loading the previous completed buffer, placing the first
  // buffer at the end of the queue, 1 means loading the last buffer at the
  // beginning of the queue, which is equal to restoring the most recent buffer
  // after using -1. If the loaded buffer can't be used by one of the attached
  // filters, the operation fails. If there's an unfinished buffer at the front
  // of the queue, interrupts the rendering and places it at the back of the
  // queue.
  OptionalError shiftBufferQueue(int shift);

  // Get the topmost/first/current buffer
  std::shared_ptr<Buffer> getTopBuffer() const;

  // Get a buffer by index
  std::shared_ptr<Buffer> getBuffer(int index) const;

  // Get all buffers
  std::list<std::shared_ptr<Buffer>> getBuffers() const;

  // Gets next inputting filter round-robin
  FilterWrapper* getNextInputtingFilter();

  // Simple getter and setter, synchronized
  uint32_t getMaxBufferHistory();

  void setMaxBufferHistory(uint32_t value);

private:

  // Move completed historic buffers back in the queue and place a new one on
  // top, matching type of the previous top buffer, cuts the tail if necessary
  // If there's an unfinished buffer on top, interrupts its rendering and
  // doesn't move anything
  void cycleBuffersNoLock();

  // Try to place a buffer from somewhere else on top of the buffer. Check if
  // all connected filters can handle the new buffer source is an optional
  // argument and points to a filter that initiated the change
  OptionalError tryReplaceBufferNoLock(std::shared_ptr<Buffer> buffer,
                                       FilterWrapper* source = nullptr);

  // The setters below change the connection between them and the filters
  FilterWrapper* getOutputtingFilterNoLock() const;

  void setOutputtingFilterNoLock(FilterWrapper* filter);

  std::vector<FilterWrapper*> getInputtingFiltersNoLock() const;

  void addInputtingFilterNoLock(FilterWrapper* filter);

  void removeInputtingFilterNoLock(FilterWrapper* filter);

  // Called by Buffer after all jobs have been completed
  void notifyCompletionNoLock(Buffer* source);

  // Called by Buffer after a job have been completed and some data have
  // been added to the buffer
  void notifySegmentCompletionNoLock(Buffer* source);

  // changes new mutex in itself and propagates the changes to and inputting filters
  void propagateSharedMutexNoLock(const SharedMutexWrapper& new_shared_mutex);

  // request outputting filter to create and propagate a new shared mutex for the pipeline
  void updatePipelineSharedMutexNoLock();

  // contained buffers
  std::list<std::shared_ptr<Buffer>> buffers;

  // connections, may or may not exist
  FilterWrapper* outputting_filter = nullptr;
  std::vector<FilterWrapper*> inputting_filters;

  // keeps track of round robin next filter choice
  uint32_t next_chosen_inputting_filter = 0;

  // Maximum length of buffer history
  uint32_t max_buffer_history;

  // shared mutex synchronizes functions that use neighboring buffers/filters,
  // local mutex synchronizes functions that don't,
  // mutexes should be locked once on the start of top level function,
  // functions with locks should not use other synchronized functions
  // non-synchronized functions have NoLock suffix
  SharedMutexWrapper shared_mutex;
  LocalMutexWrapper local_mutex;

  friend class FilterWrapper;
  friend class Buffer;
};


#endif //LUCIMALIR_BUFFERCOLLECTION_H
