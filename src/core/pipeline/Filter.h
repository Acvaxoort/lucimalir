//
// Created by rutio on 2020-04-06.
//

#ifndef LUCIMALIR_FILTER_H
#define LUCIMALIR_FILTER_H


#include <memory>
#include "FilterJob.h"
#include "Buffer.h"
#include "core/util/OptionalError.h"
#include "FilterWrapper.h"
#include "Parameter.h"
#include <core/util/ConcatConstChar.h>
#include <core/wrappers/SharedPtrParameter.h>

class Filter {
public:
  struct ParamChangeStatus {
    ParamChangeStatus(bool change_input = false, std::string error = "")
        : change_input(change_input), error(std::move(error)) {}

    bool change_input;
    std::string error;
  };

  // Gets name for display in qui, uses ordering_index
  std::string getName() const {
    return getLabel() + (" " + std::to_string(unique_id));
  }

  // Does nothing, useful for ImageExternalOutput
  virtual void waitTillCompletion();

  // Get short description identifying the filter type
  virtual const char* getLabel() const = 0;

  // Get internal type name for the filter used by lua
  virtual const char* getTypeName() const = 0;

  // Gets number of jobs currently active for this filter
  int getJobCount() const;

  // Gets the filter's current rendering progress rom 0 to 1
  virtual double getProgress() const = 0;

  // Whether the parameter exists
  virtual bool hasNumberParameter(const std::string& key);

  virtual bool hasStringParameter(const std::string& key);

  virtual bool hasViewParameter(const std::string& key);

  virtual bool hasParameter(const std::string& key);

  // Getting and setting parameters by key
  virtual double getNumberParameter(const std::string& key);

  virtual std::string getStringParameter(
      const std::string& key);

  virtual ViewWrapper getViewParameter(const std::string& key);

  virtual SharedPtrParameter getParameter(const std::string& key);

  virtual ParamChangeStatus setNumberParameter(
      const std::string& key, double value);

  virtual ParamChangeStatus setStringParameter(
      const std::string& key, const std::string& value);

  virtual ParamChangeStatus setViewParameter(
      const std::string& key, const ViewWrapper& value);

  virtual ParamChangeStatus setParameter(
      const std::string& key, SharedPtrParameter value);

  Filter() = default;

  Filter(const Filter& other);

  virtual ~Filter() = default;

  // FilterWrapper that contains this, controlled by the wrapper
  FilterWrapper* context = nullptr;
protected:
  // Easy verification of buffers, for canOutputTo and canInputFrom
  template<typename T>
  static OptionalError verifyMatrixBuffer(Buffer* b) {
    static constexpr const char prefix[] = "expected matrix array of ";
    if (dynamic_cast<MatrixArrayBuffer<T>*>(b)) {
      return true;
    } else {
      return concat<prefix, DataTypes::TypeName<T>::name>;
    };
  };

  static OptionalError verifyMatrixBufferOfFloat(Buffer* b) {
    if (dynamic_cast<MatrixArrayBuffer<DataTypes::FLOAT32>*>(b)) {
      return true;
    } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::FLOAT64>*>(b)) {
      return true;
    } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::FLOAT128>*>(b)) {
      return true;
    } else {
      return "expected matrix array of some floating point type";
    };
  };

  static OptionalError verifyMatrixBufferOfQuarternion(Buffer* b) {
    if (dynamic_cast<MatrixArrayBuffer<DataTypes::QUATERNION32>*>(b)) {
      return true;
    } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::QUATERNION64>*>(b)) {
      return true;
    } else if (dynamic_cast<MatrixArrayBuffer<DataTypes::QUATERNION128>*>(b)) {
      return true;
    } else {
      return "expected matrix array of quaternions of some floating point type";
    };
  };

  static OptionalError verifyViewBufferOfFloat(Buffer* b) {
    if (dynamic_cast<ViewBuffer<DataTypes::FLOAT32>*>(b)) {
      return true;
    } else if (dynamic_cast<ViewBuffer<DataTypes::FLOAT64>*>(b)) {
      return true;
    } else if (dynamic_cast<ViewBuffer<DataTypes::FLOAT128>*>(b)) {
      return true;
    } else {
      return "expected a view";
    };
  };

  // Return next job that can be done on this filter or nullptr
  virtual FilterJob* makeNextJob(
      std::shared_ptr<Buffer> input,
      std::shared_ptr<Buffer> output) = 0;

  // Used to assign ids, this has to be done manually, non-negative ids are
  // meant for displayable, non-working filers, negative are meant for working
  // filters, they need unique ids in order to prevent false rendering requests
  // non-working filter ids aren't actually used for anything other than
  // display
  void registerFilterId();

  void registerNegativeFilterId();

private:
  friend class FilterJob;

  friend class FilterWrapper;

  friend class BufferCollection;

  void unregisterJob(FilterJob* job);

  // Orders all active filter jobs to interrupt, called by FilterWrapper::interrupt
  void interrupt();

  // interrupt without the lock, used by the destructor
  void interruptNoLock();

  // Checks if it's safe to begin new computation (job list is empty while interrupted)
  // and returns makeNextJob, otherwise returns nullptr
  std::shared_ptr<FilterJob> getNextJob(
      std::shared_ptr<Buffer> input,
      std::shared_ptr<Buffer> output);

  // Make a copy of itself, used to make a copy the filter that will be used for rendering,
  // so that the rendering keeps using the same parameters even after adjusting it before the rendering ends
  virtual std::unique_ptr<Filter> clone() const = 0;

  // Called when changing or creating an output buffer for the filter
  // If possible, these changes should be propagated to the input instead of returning an error
  virtual OptionalError canOutputTo(Buffer* output) const = 0;

  // Called if the function above returns no error,
  // should make all necessary changes for it to output to the new buffer
  // In particular, it should propagate the changes to its input, using context.tryChangeInput
  // It should return a status message of something important had to be changed
  // Or if there was an error in propagating the changes
  virtual OptionalError adaptToOutput(
      Buffer* output,
      FilterWrapper* context) = 0;

  // Called when changing or creating an input buffer for the filter
  // These changes should not be propagated to the output and the function should return an error instead
  virtual OptionalError canInputFrom(Buffer* input) const = 0;

  // Called if the function above returns no error,
  // should make all necessary changes to itself for it to input from the new buffer
  // It shouldn't propagate the changes
  // It should return a status message of something important had to be changed
  virtual OptionalError adaptToInput(Buffer* input) = 0;

  // Make a new buffer that can be used as an input for the filter
  virtual std::shared_ptr<Buffer> makeInputBuffer() const = 0;

  // Whether the filter accepts data from outside the rendering pipeline
  // i.e. not from a buffer (e.g. a function, gui)
  virtual bool hasExternalInput() const { return false; };

  // Whether the filter sends data outside of the rendering pipeline
  // i.e. not to a buffer (e.g. screen, file)
  virtual bool hasExternalOutput() const { return false; };

  // getNextJob without locking
  std::shared_ptr<FilterJob> getNextJobNoLock(
      std::shared_ptr<Buffer> input,
      std::shared_ptr<Buffer> output);

  void disconnectJobs();

  //mutable std::mutex job_list_mutex;
  LocalMutexWrapper local_mutex;

  std::weak_ptr<Filter> this_weak_ptr;

  std::vector<std::weak_ptr<FilterJob>> jobs;
  int unique_id = 0;
  bool interrupted = false;
};

#define FILTER_HAS(name)\
if (key == #name) {\
  return true;\
} else

#define FILTER_HAS_END() { return false; }

#define FILTER_GET(name)\
if (key == #name) {\
  return params.name;\
} else

#define FILTER_GET_FIELD(name)\
if (key == #name) {\
  return name;\
} else

#define FILTER_GET_ENUM(name)\
if (key == #name) {\
  auto& param_ref = params.name;

#define FILTER_GET_FIELD_ENUM(name)\
if (key == #name) {\
  auto& param_ref = name;

#define FILTER_GET_ENUM_VALUE(check_value, enum_value)\
if (param_ref == enum_value) {\
  return check_value;\
} else

#define FILTER_GET_ENUM_END(ret) { return ret; } } else

#define FILTER_GET_END_NUM() { return 0.0; }
#define FILTER_GET_END_PAR() { return nullptr; }
#define FILTER_GET_END_VIEW() { return {}; }
#define FILTER_GET_END_STRING() { return ""; }

#define FILTER_SET(name, ret)\
if (key == #name) {\
  params.name = value;\
  return ret;\
} else

#define FILTER_SET_FIELD(name, ret)\
if (key == #name) {\
  name = value;\
  return ret;\
} else

#define FILTER_SET_POSITIVE(name, ret)\
if (key == #name) {\
  if (value > 0) {\
    params.name = value;\
    return ret;\
  } else {\
    return {false, "expected a positive value"};\
  }\
} else

#define FILTER_SET_FIELD_POSITIVE(name, ret)\
if (key == #name) {\
  if (value > 0) {\
    name = value;\
    return ret;\
  } else {\
    return {false, "expected a positive value"};\
  }\
} else

#define FILTER_SET_ENUM(name, change_input)\
if (key == #name) {\
  auto& param_ref = params.name;\
  auto ret_ref = change_input;

#define FILTER_SET_FIELD_ENUM(name, change_input)\
if (key == #name) {\
  auto& param_ref = name;\
  auto ret_ref = change_input;

#define FILTER_SET_ENUM_VALUE(check_value, enum_value)\
if (value == check_value) {\
  param_ref = enum_value;\
  return ret_ref;\
} else

#define FILTER_SET_ENUM_END(error) { return {false, error}; } } else

#define FILTER_SET_END() { return {}; }

#define FILTER_HAS_ALL_DIMENSIONS() FILTER_HAS(width) FILTER_HAS(height) FILTER_HAS(num_layers)
#define FILTER_GET_ALL_DIMENSIONS() FILTER_GET_FIELD(width) FILTER_GET_FIELD(height) FILTER_GET_FIELD(num_layers)
#define FILTER_SET_ALL_DIMENSIONS() FILTER_SET_FIELD(width, true) FILTER_SET_FIELD(height, true) FILTER_SET_FIELD(num_layers, true)

#endif //LUCIMALIR_FILTER_H
