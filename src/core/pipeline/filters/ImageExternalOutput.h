//
// Created by rutio on 2020-05-30.
//

#ifndef LUCIMALIR_IMAGEEXTERNALOUTPUT_H
#define LUCIMALIR_IMAGEEXTERNALOUTPUT_H


#include <core/pipeline/Filter.h>
#include <core/util/ThreadBlocker.h>
#include <core/util/PartitionedProcessingTracker.h>

class ImageExternalOutput : public Filter {
public:
  int width = 1920;
  int height = 1080;
  int num_layers = 1;

  bool save_files = false;

  std::string base_filename = "image";

  ImageExternalOutput();

  class Job : public FilterJob {
  public:
    Job(std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output);

    void complete() override;

    std::shared_ptr<PartitionedProcessingTracker> processing_tracker;
    int layer_index;
    std::string filename;
  };

  void waitTillCompletion() override;

  std::unique_ptr<Filter> clone() const override;

  double getProgress() const override;

  OptionalError canOutputTo(Buffer* output) const override;

  OptionalError adaptToOutput(Buffer* output, FilterWrapper* context) override;

  OptionalError canInputFrom(Buffer* input) const override;

  OptionalError adaptToInput(Buffer* input) override;

  std::shared_ptr<Buffer> makeInputBuffer() const override;

  bool hasExternalOutput() const override;

  const char* getLabel() const override;

  const char* getTypeName() const override;

  bool hasNumberParameter(const std::string& key) override;

  bool hasStringParameter(const std::string& key) override;

  double getNumberParameter(const std::string& key) override;

  std::string getStringParameter(const std::string& key) override;

  ParamChangeStatus
  setNumberParameter(const std::string& key, double value) override;

  ParamChangeStatus
  setStringParameter(const std::string& key, const std::string& value) override;

protected:
  FilterJob* makeNextJob(
      std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output) override;

private:
  std::shared_ptr<ThreadBlocker> completion_notifier;
  std::shared_ptr<PartitionedProcessingTracker> processing_tracker;

  std::vector<std::string> filenames;
  int num_input_layers = 0;
};


#endif //LUCIMALIR_IMAGEEXTERNALOUTPUT_H
