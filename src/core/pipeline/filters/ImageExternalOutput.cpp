//
// Created by rutio on 2020-05-30.
//

#include <core/util/PngOutput.h>
#include "ImageExternalOutput.h"


ImageExternalOutput::ImageExternalOutput()
    : completion_notifier(new ThreadBlocker()) {
}

void ImageExternalOutput::waitTillCompletion() {
  if (context->hasFutureRenderingRequest()) {
    completion_notifier->wait();
  }
}

std::unique_ptr<Filter> ImageExternalOutput::clone() const {
  return std::make_unique<ImageExternalOutput>(*this);
}

double ImageExternalOutput::getProgress() const {
  if (num_input_layers == 0) {
    return 0.0;
  }
  return (processing_tracker->getNumDispatched() - (getJobCount() * 0.5)) /
         num_input_layers;
}


OptionalError ImageExternalOutput::canOutputTo(Buffer* output) const {
  return "this filter doesn't output";
}

OptionalError ImageExternalOutput::adaptToOutput(
    Buffer* output,
    FilterWrapper* context) {
  return true;
}

OptionalError ImageExternalOutput::canInputFrom(Buffer* input) const {
  return verifyMatrixBuffer<DataTypes::RGBA>(input);
}

OptionalError ImageExternalOutput::adaptToInput(Buffer* input) {
  auto& input_buf = dynamic_cast<MatrixArrayBuffer<DataTypes::RGBA>&>(*input);
  width = (int) input_buf.getWidth();
  height = (int) input_buf.getHeight();
  num_layers = (int) input_buf.getNumLayers();
  return true;
}

std::shared_ptr<Buffer> ImageExternalOutput::makeInputBuffer() const {
  return Buffer::makeSharedBuffer<MatrixArrayBuffer<DataTypes::RGBA>>(
      num_layers, width, height);
}

bool ImageExternalOutput::hasExternalOutput() const {
  return true;
}

const char* ImageExternalOutput::getLabel() const {
  return "Image Output";
}

const char* ImageExternalOutput::getTypeName() const {
  return "outputs.image";
}

FilterJob*
ImageExternalOutput::makeNextJob(
    std::shared_ptr<Buffer> input,
    std::shared_ptr<Buffer> output) {
  if (!processing_tracker) {
    processing_tracker = std::make_shared<PartitionedProcessingTracker>(
        num_layers);
  }
  if (processing_tracker->didDispatchAll()) {
    return nullptr;
  }
  if (!input->isCompleted()) {
    return nullptr;
  }
  auto& input_buf = dynamic_cast<MatrixArrayBuffer<DataTypes::RGBA>&>(*input);
  num_input_layers = (int) input_buf.getNumLayers();
  int index = processing_tracker->getNextIndex();
  auto ret = new Job(std::move(input), std::move(output));
  ret->processing_tracker = processing_tracker;
  ret->layer_index = index;
  if (save_files && filenames.empty()) {
    filenames = PngOutput::registerFilenames(base_filename, num_input_layers);
    ret->filename = std::move(filenames[index]);
  } else {
    ret->filename = "";
  }
  if (processing_tracker->didDispatchAll()) {
    completion_notifier->notify();
  }
  return ret;
}

bool ImageExternalOutput::hasNumberParameter(const std::string& key) {
  FILTER_HAS_ALL_DIMENSIONS() //
  FILTER_HAS_END()
}

bool ImageExternalOutput::hasStringParameter(const std::string& key) {
  FILTER_HAS(save_files) //
  FILTER_HAS(base_filename) //
  FILTER_HAS_END()
}

double ImageExternalOutput::getNumberParameter(const std::string& key) {
  FILTER_GET_ALL_DIMENSIONS() //
  FILTER_GET_END_NUM()
}

std::string ImageExternalOutput::getStringParameter(const std::string& key) {
  FILTER_GET_FIELD_ENUM(save_files) //
    FILTER_GET_ENUM_VALUE("__true", true) //
    FILTER_GET_ENUM_VALUE("__false", false) //
  FILTER_GET_ENUM_END("") //
  FILTER_GET_FIELD(base_filename) //
  FILTER_GET_END_STRING()
}

Filter::ParamChangeStatus
ImageExternalOutput::setNumberParameter(const std::string& key, double value) {
  FILTER_SET_ALL_DIMENSIONS() //
  FILTER_SET_END()
}

Filter::ParamChangeStatus ImageExternalOutput::setStringParameter(
    const std::string& key, const std::string& value) {
  FILTER_SET_FIELD_ENUM(save_files, false) //
    FILTER_SET_ENUM_VALUE("__true", true) //
    FILTER_SET_ENUM_VALUE("__false", false) //
  FILTER_SET_ENUM_END("") //
  FILTER_SET_FIELD(base_filename, false) //
  FILTER_SET_END()
}

ImageExternalOutput::Job::Job(
    std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output)
    : FilterJob(std::move(input), std::move(output)) {}

void ImageExternalOutput::Job::complete() {
  auto& input_array_buffer = dynamic_cast<MatrixArrayBuffer<DataTypes::RGBA>&>(*input);
  auto layer = input_array_buffer.getView().getLayerView(layer_index);
  if (!filename.empty()) {
    PngOutput::saveFile(filename, (int) layer.width, (int) layer.height,
                        layer.data);
  }
  processing_tracker->notifyProcessingEnd(layer_index);
  if (processing_tracker->didProcessAll()) {
    auto origin_shared = origin.lock();
    if (origin_shared) {
      if (origin_shared->context) {
        origin_shared->context->completeRendering();
      }
    }
  }
}

