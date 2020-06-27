//
// Created by rutio on 2020-04-06.
//

#ifndef LUCIMALIR_BUFFER_H
#define LUCIMALIR_BUFFER_H

#include <vector>
#include <set>
#include <mutex>
#include <memory>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <core/util/LocalMutexWrapper.h>
#include <core/util/RenderingView.h>
#include <core/util/DataTypes.h>

class BufferCollection;

class Buffer {
public:
  // T must be class derived from buffer
  // A thread that wants to render to this buffer can use getManagedThisPointer and this will ensure that
  // the thread will be able to finish rendering without crashing
  template<class T, class... Args>
  static std::shared_ptr<Buffer> makeSharedBuffer(Args&& ... args) {
    std::shared_ptr<Buffer> new_this = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    new_this->this_managed_pointer = new_this;
    return std::move(new_this);
  }

  template<template<class> class BufferType, class... Args>
  static std::shared_ptr<Buffer>
  makeSharedBufferOfFloats(DataTypes::FloatingPrecision precision, Args&& ... args) {
    std::shared_ptr<Buffer> new_this;
    switch (precision) {
      case DataTypes::FP32:
        new_this = std::make_shared<BufferType<DataTypes::FLOAT32>>(std::forward<Args>(args)...);
        break;
      case DataTypes::FP64:
        new_this = std::make_shared<BufferType<DataTypes::FLOAT64>>(std::forward<Args>(args)...);
        break;
      case DataTypes::FP128:
        new_this = std::make_shared<BufferType<DataTypes::FLOAT128>>(std::forward<Args>(args)...);
        break;
    }
    new_this->this_managed_pointer = new_this;
    return std::move(new_this);
  }

  template<template<class> class BufferType, class... Args>
  static std::shared_ptr<Buffer>
  makeSharedBufferOfQuaternions(DataTypes::FloatingPrecision precision, Args&& ... args) {
    std::shared_ptr<Buffer> new_this;
    switch (precision) {
      case DataTypes::FP32:
        new_this = std::shared_ptr<BufferType<DataTypes::QUATERNION32>>(new BufferType(std::forward<Args>(args)...));
        break;
      case DataTypes::FP64:
        new_this = std::shared_ptr<BufferType<DataTypes::QUATERNION64>>(new BufferType(std::forward<Args>(args)...));
        break;
      case DataTypes::FP128:
        new_this = std::shared_ptr<BufferType<DataTypes::QUATERNION128>>(new BufferType(std::forward<Args>(args)...));
        break;
    }
    new_this->this_managed_pointer = new_this;
    return std::move(new_this);
  }

  template<template<class, class> class Functor, class... Args>
  static void callWithPrecision(Buffer* buf, Args&& ... args) {
    switch (getPrecision(buf).value()) {
      case DataTypes::FP32: {
        Functor<DataTypes::FLOAT32, DataTypes::COMPLEX32> f;
        f(std::forward<Args>(args)...);
      }
        break;
      case DataTypes::FP64: {
        Functor<DataTypes::FLOAT64, DataTypes::COMPLEX64> f;
        f(std::forward<Args>(args)...);
      }
        break;
      case DataTypes::FP128: {
        Functor<DataTypes::FLOAT128, DataTypes::COMPLEX128> f;
        f(std::forward<Args>(args)...);
      }
        break;
    }
  }

  static std::optional<DataTypes::FloatingPrecision> getPrecision(Buffer* buf);


  // Get the right shared_ptr to this
  std::shared_ptr<Buffer> getManagedThisPointer();

  // Check if the other buffer has the same type, which may include
  // not only the class but also some parameters
  virtual bool isTheSameType(const Buffer* other) const = 0;

  // Make a new buffer of the same type
  virtual std::shared_ptr<Buffer> makeNewOfSameType() const = 0;

  // Get a descriptive name of the type of the buffer, including important parameters
  virtual std::string getTypeDescription() const = 0;

  // Whether the buffer is considered completed and can be used as an input
  bool isCompleted() const;

  // Get number of segments that have been computed, this may or may not be used by a connected filter
  int getNumCompletedSegments() const;

  // Get total number of segments in this buffer, this may or may not be used by a connected filter
  int getNumSegments() const;

  // Set the buffer as fresh, as if it contained no data, after interruption or changing parameters
  void resetCompletion();

  // Notify completion of all jobs, redundant if notifySegmentCompletion is called for every segment
  void notifyCompletion();

  // Notify of completion of a segment
  void notifySegmentCompletion(int64_t index);

  constexpr static int64_t SEGMENT_HEIGHT = 8;

  virtual ~Buffer() = default;

protected:
  LocalMutexWrapper local_mutex;

  int num_segments = 1;
  int computed_segments = 0;
  std::set<int64_t> stray_computed_segments;

  Buffer() = default;

private:
  // synchronized getter/setter
  BufferCollection* getOwner();

  void setOwner(BufferCollection* new_owner);

  std::weak_ptr<Buffer> this_managed_pointer;
  BufferCollection* owner = nullptr;

  friend class BufferCollection;
  friend class Filter;
};

template<class T>
class ViewBuffer : public Buffer {
public:
  bool isTheSameType(const Buffer* other) const override {
    return dynamic_cast<const ViewBuffer<T>*>(other);
  }

  std::shared_ptr<Buffer> makeNewOfSameType() const override {
    return makeSharedBuffer<ViewBuffer<T>>();
  }

  std::string getTypeDescription() const override {
    std::stringstream ss;
    ss << DataTypes::TypeName<T>::name << " region";
    return ss.str();
  }

  RenderingView<T> view;

  friend class Buffer;
};

template<class T>
struct MatrixLayerView {
  std::shared_ptr<Buffer> origin;
  T* data;
  T* maximum;
  int64_t width;
  int64_t height;
  int64_t data_length;
  int64_t start_offset;
  int64_t end_offset;

  inline T& operator[](int64_t i) {
    return data[i];
  }

  inline T& at(int64_t i) {
    return data[i];
  }

  inline T& at(int64_t x, int64_t y) {
    return data[y * width + x];
  }
};

template<class T>
struct MatrixLayerArrayView {
  MatrixLayerArrayView(std::shared_ptr<Buffer> origin, std::vector<T*> data, std::vector<T*> maximum,
                       int64_t width, int64_t height, int64_t start_offset, int64_t end_offset)
      : origin(std::move(origin)), data(std::move(data)), maximum(std::move(maximum)), width(width), height(height),
        data_length(width * height), start_offset(start_offset), end_offset(end_offset),
        num_layers(this->data.size()) {}

  inline T& at(int64_t index, int64_t x, int64_t y) {
    return data[index][y * width + x];
  }

  inline T& at(int64_t index, int64_t i) {
    return data[index][i];
  }

  inline MatrixLayerView<T> getLayerView(int64_t index) {
    return {origin, data[index], maximum[index], width, height, data_length, start_offset, end_offset};
  }

  std::shared_ptr<Buffer> origin;
  std::vector<T*> data;
  std::vector<T*> maximum;
  int64_t width;
  int64_t height;
  int64_t data_length; // width * height
  int64_t start_offset; // beginning of the entire data block in memory, offset by start of this block (0 or lower)
  int64_t end_offset; // end of the entire data block in memory, offset by start of this block (data_length or higher)
  int64_t num_layers; // data.size()
};

template<class T>
class MatrixArrayBuffer : public Buffer {
public:
  MatrixLayerArrayView<T> getSegmentView(int64_t index) {
    allocate();
    std::vector<T*> return_data(num_layers);
    std::vector<T*> return_maximum(num_layers);
    int64_t offset = index * width * SEGMENT_HEIGHT;
    for (int64_t i = 0; i < num_layers; ++i) {
      return_data[i] = data[i].data() + offset;
      return_maximum[i] = maximum.data() + i;
    }
    int64_t segment_height = std::min((index + 1) * SEGMENT_HEIGHT, height) - index * SEGMENT_HEIGHT;
    return {getManagedThisPointer(), std::move(return_data), std::move(return_maximum),
            width, segment_height, -offset, data_length - offset};
  }

  MatrixLayerArrayView<T> getView() {
    allocate();
    std::vector<T*> return_data(num_layers);
    std::vector<T*> return_maximum(num_layers);
    for (int64_t i = 0; i < num_layers; ++i) {
      return_data[i] = data[i].data();
      return_maximum[i] = maximum.data() + i;
    }
    return {getManagedThisPointer(), std::move(return_data), std::move(return_maximum), width, height, 0, data_length};
  }

  bool isTheSameType(const Buffer* other) const override {
    if (auto p = dynamic_cast<const MatrixArrayBuffer<T>*>(other)) {
      if (width == p->width && height == p->height && num_layers == p->num_layers) {
        return true;
      }
    }
    return false;
  }

  bool isTheSameDimensions(const Buffer* other) const {
    if (auto p = dynamic_cast<const MatrixArrayBuffer<T>*>(other)) {
      if (width == p->width && height == p->height) {
        return true;
      }
    }
    return false;
  }

  std::shared_ptr<Buffer> makeNewOfSameType() const override {
    return makeSharedBuffer<MatrixArrayBuffer<T>>(num_layers, width, height);
  }

  std::string getTypeDescription() const override {
    std::stringstream ss;
    ss << DataTypes::TypeName<T>::name << " " << width << "x" << height << "matrix array[" << num_layers << "]";
    return ss.str();
  }

  inline int64_t getWidth() {
    return width;
  }

  inline int64_t getHeight() {
    return height;
  }

  inline int64_t getNumLayers() {
    return num_layers;
  }

protected:
  friend class Buffer;

  MatrixArrayBuffer(int64_t num_layers, int64_t width, int64_t height)
      : data(0),
        maximum(0),
        width(width),
        height(height),
        data_length(width * height),
        num_layers(num_layers) {
    num_segments = (height + SEGMENT_HEIGHT - 1) / SEGMENT_HEIGHT;
  }

  void allocate() {
    std::lock_guard<std::mutex> lock(allocation_mutex);
    if (data.empty()) {
      data.resize(num_layers);
      maximum.resize(num_layers);
      for (auto& m : data) {
        m.resize(data_length);
      }
    }
  }

  std::vector<std::vector<T>> data;
  std::vector<T> maximum;
  int64_t width;
  int64_t height;
  int64_t data_length;
  int64_t num_layers;
  std::mutex allocation_mutex;

};


#endif //LUCIMALIR_BUFFER_H
