//
// Created by rutio on 2020-04-23.
//

#include <core/pipeline/parameters/ConstantParameter.h>
#include "MandelbrotJulia.h"


double MandelbrotJulia::getProgress() const {
  if (num_segments == 0) {
    return 0.0;
  }
  return (next_segment - (getJobCount() * 0.5)) / num_segments;
}

std::unique_ptr<Filter> MandelbrotJulia::clone() const {
  return std::make_unique<MandelbrotJulia>(*this);
}

OptionalError MandelbrotJulia::canOutputTo(Buffer* output) const {
  return Filter::verifyMatrixBuffer<DataTypes::INTEGER>(output);
}

OptionalError MandelbrotJulia::adaptToOutput(
    Buffer* output,
    FilterWrapper* context) {
  return true;
}

OptionalError MandelbrotJulia::canInputFrom(Buffer* input) const {
  return Filter::verifyViewBufferOfFloat(input);
}

OptionalError MandelbrotJulia::adaptToInput(Buffer* input) {
  params.precision = Buffer::getPrecision(input).value();
  return true;
}

std::shared_ptr<Buffer> MandelbrotJulia::makeInputBuffer() const {
  return Buffer::makeSharedBufferOfFloats<ViewBuffer>(params.precision);
}

const char* MandelbrotJulia::getTypeName() const {
  return "fractals.basic";
}

const char* MandelbrotJulia::getLabel() const {
  return "Basic fractal renderer";
}

FilterJob*
MandelbrotJulia::makeNextJob(
    std::shared_ptr<Buffer> input,
    std::shared_ptr<Buffer> output) {
  num_segments = output->getNumSegments();
  if (input->isCompleted() && next_segment < num_segments) {
    auto p = new Job(std::move(input), std::move(output));
    p->segment_index = next_segment;
    p->params = params;
    next_segment++;
    return p;
  }
  return nullptr;
}

bool MandelbrotJulia::hasNumberParameter(const std::string& key) {
  FILTER_HAS(precision) //
  FILTER_HAS_END()
}

bool MandelbrotJulia::hasParameter(const std::string& key) {
  FILTER_HAS(iterations) //
  FILTER_HAS_END()
}

double
MandelbrotJulia::getNumberParameter(const std::string& key) {
  FILTER_GET_ENUM(precision) //
    FILTER_GET_ENUM_VALUE(32, DataTypes::FP32) //
    FILTER_GET_ENUM_VALUE(64, DataTypes::FP64) //
  FILTER_GET_ENUM_END(DataTypes::FP64) //
  FILTER_GET_END_NUM()
}

SharedPtrParameter
MandelbrotJulia::getParameter(const std::string& key) {
  FILTER_GET(iterations) //
  FILTER_GET_END_PAR()
}

Filter::ParamChangeStatus MandelbrotJulia::setNumberParameter(
    const std::string& key, double value) {
  FILTER_SET_ENUM(precision, true) //
    FILTER_SET_ENUM_VALUE(32, DataTypes::FP32) //
    FILTER_SET_ENUM_VALUE(64, DataTypes::FP64) //
  FILTER_SET_ENUM_END("valid values are 32 and 64") //
  FILTER_SET_END()
}

Filter::ParamChangeStatus MandelbrotJulia::setParameter(
    const std::string& key, SharedPtrParameter value) {
  FILTER_SET(iterations, false) //
  FILTER_SET_END()
}

MandelbrotJulia::Job::Job(std::shared_ptr<Buffer> input,
    std::shared_ptr<Buffer> output)
    : FilterJob(std::move(input), std::move(output)) {}

using std::norm;

#ifdef ENABLE_FLOAT128
using boost::multiprecision::norm;
#endif

template<class FloatType, class ComplexType>
struct Functor {
  void operator()(
      Buffer* input, Buffer* output, int segment_index,
      const MandelbrotJulia::Params& params) {
    auto& input_buf = dynamic_cast<ViewBuffer<FloatType>&>(*input);
    auto& output_buf = dynamic_cast<MatrixArrayBuffer<DataTypes::INTEGER>&>(*output);
    auto output_segment = output_buf.getSegmentView(segment_index);
    int width = output_buf.getWidth();
    int height = output_buf.getHeight();
    int height_offset = segment_index * Buffer::SEGMENT_HEIGHT;
    RenderingViewCoordinateGenerator<FloatType> coordinate_generator(
        input_buf.view, width, height);
    for (int layer = 0; layer < output_segment.num_layers; ++layer) {
      auto output_segment_layer = output_segment.getLayerView(layer);
      int layer_iterations = (int) params.iterations.parameter->getValue(
          layer,
          output_segment.num_layers);
      *output_segment_layer.maximum = layer_iterations;
      int x = 0;
      int y = height_offset;
      for (int64_t j = 0; j < output_segment_layer.data_length; ++j) {
        Vec4D<FloatType> pos = coordinate_generator.getCoordinates(x, y);
        ComplexType c(pos[0], pos[1]);
        ComplexType z(pos[2], pos[3]);
        int i;
        for (i = 0; i < layer_iterations; ++i) {
          if (norm(z) > (FloatType) 4) {
            break;
          }
          z *= z;
          z += c;
        }
        output_segment_layer[j] = i;
        x++;
        if (x == width) {
          y++;
          x = 0;
        }
      }
    }
    output->notifySegmentCompletion(segment_index);
  }
};

void MandelbrotJulia::Job::complete() {
  Buffer::callWithPrecision<Functor>(input.get(), input.get(), output.get(),
                                     segment_index, params);
}
