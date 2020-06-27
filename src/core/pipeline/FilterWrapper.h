//
// Created by rutio on 2020-04-06.
//

#ifndef LUCIMALIR_FILTERWRAPPER_H
#define LUCIMALIR_FILTERWRAPPER_H

#include <memory>
#include <core/util/OptionalError.h>
#include <core/util/SharedMutexWrapper.h>
#include <core/util/LocalMutexWrapper.h>
#include "Buffer.h"
#include "FilterJob.h"

class Filter;

class BufferCollection;

// Functions with NoLock suffix are meant for internal use
// other functions have all the required synchronization
class FilterWrapper {
public:
  // -------------------------------------------------------------------------
  // Functions for external usage
  // -------------------------------------------------------------------------

  // Create the wrapper, it should always be created before using the filter
  // the filter should not be nullptr
  //explicit FilterWrapper(std::shared_ptr<Filter> filter);

  explicit FilterWrapper(Filter* filter);

  // Simple getters, synchronized where needed
  bool isCurrentlyRendering() const;

  bool isAnExternalInput() const;

  bool isAnExternalOutput() const;

  bool isConnectedToExternalOutput() const;

  bool isFilterGroupRoot() const;

  bool isPipelineRoot() const;

  bool isStray() const;

  bool isAutoRequestOnChanges() const;

  bool hasFutureRenderingRequest() const;

  // Set whether the filter requests rendering if it changes
  void setAutoRequestOnChanges(bool value);

  // Whether the new_filter fits the current inputs and outputs
  OptionalError canReplaceFilter(Filter* new_filter) const;

  // Try to replace with the new_filter, should be called after canReplaceFilter
  OptionalError replaceFilter(std::shared_ptr<Filter> new_filter);

  // Get weak pointer to wrapped filter
  // should be used for informative purposes and to change its parameters
  std::weak_ptr<Filter> getFilter();

  // Same as above but not wrapped, is meant for usage with lua api,
  // object lifetime should be considered
  Filter* getFilterRaw() const;

  // Get weak pointer to the copy of filter that is responsible for rendering,
  // should be used for informative purposes only
  std::weak_ptr<const Filter> getWorkingFilter() const;

  // Calls interrupt on the current working filter
  void interrupt();

  // Make the filter start trying to compute and give jobs to the worker threads
  // Whether the jobs will be given depends on the wrapped filter
  void requestRendering(int origin_id = -1);

  // Whether the buffer can be connected to prev (previous filter, earlier in
  // the pipeline). If there are any buffers attached in the way, tries to
  // reuse them
  OptionalError canConnectTo(FilterWrapper* prev) const;

  // Executed if above is true, creates the connection, optionally adapting
  // necessary filters and creating necessary buffers and setting connections
  // in buffers
  OptionalError connectTo(FilterWrapper* prev);

  // Remove the connection with its input buffer and filter,
  // will also destroy connection from the buffer side and update related
  // filters' connection status
  void disconnectInput();

  // Remove the connection with its output buffer and filter,
  // will also destroy connection from the buffer side and update related
  // filters' connection status
  void disconnectOutput();

  // Get the buffer used by the filter as input, can be null
  std::weak_ptr<BufferCollection> getInputBuffer();

  // Get the buffer used by the filter as output, can be null
  std::weak_ptr<BufferCollection> getOutputBuffer();

  // requestRendering but only if auto_request_on_changes
  void notifyFilterInternalChanges();

  // Tries to change the input and returns an error if it's not possible
  OptionalError requestChangingInput();

  // -------------------------------------------------------------------------
  // Functions that are safe but not useful outside of scheduler/filters
  // -------------------------------------------------------------------------

  // Gets one of the next filters in the pipeline
  FilterWrapper* getNextFilter();

  // Gets a filter job from the working filter if possible
  std::shared_ptr<FilterJob> getJob();

  // Make the filter stop trying to compute
  void completeRendering();

  // Try to replace the inner buffer of the input, using
  // BufferCollection::tryReplaceBuffer, if it's not possible, the filter will
  // disconnect from the input using disconnectInput
  OptionalError tryChangeInputNoLock(std::shared_ptr<Buffer> new_buffer);

private:
  // -------------------------------------------------------------------------
  // Functions for internal use (filter/buffer wrappers)
  // -------------------------------------------------------------------------

  // interrupt without locking the mutex
  void interruptNoLock();

  //  requestRendering without locking the mutex
  void requestRenderingNoLock(int origin_id = -1);

  // completeRendering without locking the mutex
  void completeRenderingNoLock();

  // disconnectInput without locking the mutex
  void disconnectInputNoLock();

  // disconnectOutput without locking the mutex
  void disconnectOutputNoLock();

  // updates the three flags below, called automatically by functions
  // in FilterWrapper when necessary
  void updateConnectionStatusNoLock();

  // changes new mutex in itself and propagates the changes to and inputting
  // filters
  void propagateSharedMutexNoLock(const SharedMutexWrapper& new_shared_mutex);

  // request outputting filter to create and propagate a new shared mutex for
  // the pipeline
  void updatePipelineSharedMutexNoLock();

  // Propagate future rendering request to all subsequent filters
  void propagateFutureRenderingRequestNoLock(bool value);

  // isAnExternalInput without locking the mutex
  bool isAnExternalInputNoLock() const;

  // isAnExternaloutput without locking the mutex
  bool isAnExternalOutputNoLock() const;

  // always exists
  std::shared_ptr<Filter> filter;

  // connections, may or may not exist
  std::shared_ptr<BufferCollection> input;
  std::shared_ptr<BufferCollection> output;

  // exist if currently working
  std::shared_ptr<Filter> working_filter;
  std::shared_ptr<Buffer> working_input;
  std::shared_ptr<Buffer> working_output;

  // unique_id of the filter currently working
  int requesting_filter_id = -1;

  // connectivity flags, automatically updated on connection/disconnection
  bool is_connected_to_external_output;
  bool is_filter_group_root;
  bool is_stray;

  // set if a request from a new filter was received during working
  bool postponed_rendering_request;

  // set if a request exists earlier in the pipeline
  bool future_rendering_request;

  // whether filter will
  bool auto_request_on_changes;

  // shared mutex synchronizes functions that use neighboring buffers/filters,
  // local mutex synchronizes functions that don't,
  // mutexes should be locked once on the start of top level function,
  // functions with locks should not use other synchronized functions
  // non-synchronized functions have NoLock suffix
  mutable SharedMutexWrapper shared_mutex;
  LocalMutexWrapper local_mutex;

  friend class BufferCollection;
};


#endif //LUCIMALIR_FILTERWRAPPER_H
