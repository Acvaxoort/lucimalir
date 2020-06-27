//
// Created by rutio on 2020-05-30.
//

#include <core/pipeline/parameters/ConstantParameter.h>
#include "IntegerDataRenderer.h"


std::unique_ptr<Filter> IntegerDataRenderer::clone() const {
  return std::make_unique<IntegerDataRenderer>(*this);
}

const char* IntegerDataRenderer::getTypeName() const {
  return "renderers.basic";
}

const char* IntegerDataRenderer::getLabel() const {
  return "Basic fractal data renderer";
}

NonContextualFilterJob<DataTypes::INTEGER, DataTypes::RGBA>*
IntegerDataRenderer::makeNextJobNonContextual(
    std::shared_ptr<Buffer> input,
    std::shared_ptr<Buffer> output) {
  auto ret = new Job(std::move(input), std::move(output));
  ret->params = params;
  return ret;
}

bool IntegerDataRenderer::hasNumberParameter(const std::string& key) {
  FILTER_HAS_ALL_DIMENSIONS() //
  FILTER_HAS_END()
}

bool IntegerDataRenderer::hasStringParameter(const std::string& key) {
  FILTER_HAS(color_model) //
  FILTER_HAS_END()
}

bool IntegerDataRenderer::hasParameter(const std::string& key) {
  FILTER_HAS(low_par1) //
  FILTER_HAS(low_par2) //
  FILTER_HAS(low_par3) //
  FILTER_HAS(high_par1) //
  FILTER_HAS(high_par2) //
  FILTER_HAS(high_par3) //
  FILTER_HAS(input_gamma) //
  FILTER_HAS(output_gamma) //
  FILTER_HAS_END()
}

double IntegerDataRenderer::getNumberParameter(const std::string& key) {
  FILTER_GET_ALL_DIMENSIONS() //
  FILTER_GET_END_NUM()
}

std::string IntegerDataRenderer::getStringParameter(const std::string& key) {
  FILTER_GET_ENUM(color_model) //
    FILTER_GET_ENUM_VALUE("RGB", Colors::RGB) //
    FILTER_GET_ENUM_VALUE("HSV", Colors::HSV) //
  FILTER_GET_ENUM_END("") //
  FILTER_GET_END_STRING()
}

SharedPtrParameter IntegerDataRenderer::getParameter(const std::string& key) {
  FILTER_GET(low_par1) //
  FILTER_GET(low_par2) //
  FILTER_GET(low_par3) //
  FILTER_GET(high_par1) //
  FILTER_GET(high_par2) //
  FILTER_GET(high_par3) //
  FILTER_GET(input_gamma) //
  FILTER_GET(output_gamma) //
  FILTER_GET_END_PAR()
}

Filter::ParamChangeStatus
IntegerDataRenderer::setNumberParameter(const std::string& key, double value) {
  FILTER_SET_ALL_DIMENSIONS() //
  FILTER_SET_END()
}

Filter::ParamChangeStatus IntegerDataRenderer::setStringParameter(
    const std::string& key, const std::string& value) {
  FILTER_SET_ENUM(color_model, false) //
    FILTER_SET_ENUM_VALUE("RGB", Colors::RGB) //
    FILTER_SET_ENUM_VALUE("HSV", Colors::HSV) //
  FILTER_SET_ENUM_END(R"(valid values are "RGB" and "HSV")") //
  FILTER_SET_END()
}

Filter::ParamChangeStatus IntegerDataRenderer::setParameter(
    const std::string& key, SharedPtrParameter value) {
  FILTER_SET(low_par1, false) //
  FILTER_SET(low_par2, false) //
  FILTER_SET(low_par3, false) //
  FILTER_SET(high_par1, false) //
  FILTER_SET(high_par2, false) //
  FILTER_SET(high_par3, false) //
  FILTER_SET(input_gamma, false) //
  FILTER_SET(output_gamma, false) //
  FILTER_SET_END()
}

IntegerDataRenderer::Job::Job(
    std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output)
    : NonContextualFilterJob<DataTypes::INTEGER, DataTypes::RGBA>(
    std::move(input), std::move(output)) {
}

template<bool use_gamma>
inline double correctValue(double value, double gamma) {
  if constexpr(use_gamma) {
    return std::pow(value, gamma);
  } else {
    return value;
  }
}

template<bool use_gamma>
inline DataTypes::RGBA makeRGB(double r, double g, double b, double gamma) {
  if constexpr(use_gamma) {
    return Colors::convertRGB(r, g, b, gamma);
  } else {
    return Colors::convertRGB(r, g, b);
  }
}

template<bool use_gamma>
inline DataTypes::RGBA makeHSV(double r, double g, double b, double gamma) {
  if constexpr(use_gamma) {
    return Colors::convertHSV(r, g, b, gamma);
  } else {
    return Colors::convertHSV(r, g, b);
  }
}

template<Colors::ColorModel color_model, bool use_in_gamma, bool use_out_gamma>
inline void process(
    MatrixLayerView<DataTypes::INTEGER>& in,
    MatrixLayerView<DataTypes::RGBA>& out,
    double low_p1, double low_p2, double low_p3,
    double high_p1, double high_p2, double high_p3,
    double in_gamma, double out_gamma) {
  for (int i = 0; i < in.data_length; ++i) {
    double mul2 = correctValue<use_in_gamma>(
        in.data[i] / (double) *in.maximum, in_gamma);
    double mul1 = 1.0 - mul2;
    if constexpr (color_model == Colors::RGB) {
      out[i] = makeRGB<use_out_gamma>(
          mul1 * low_p1 + mul2 * high_p1,
          mul1 * low_p2 + mul2 * high_p2,
          mul1 * low_p3 + mul2 * high_p3,
          out_gamma
      );
    } else {
      out[i] = makeHSV<use_out_gamma>(
          mul1 * low_p1 + mul2 * high_p1,
          mul1 * low_p2 + mul2 * high_p2,
          mul1 * low_p3 + mul2 * high_p3,
          out_gamma
      );
    }
  }
}


void IntegerDataRenderer::Job::completeNonContextual(
    MatrixLayerArrayView<DataTypes::INTEGER> input,
    MatrixLayerArrayView<DataTypes::RGBA> output) {
  for (int layer = 0; layer < input.num_layers; ++layer) {
    auto input_layer = input.getLayerView(layer);
    auto output_layer = output.getLayerView(layer);
    double low_p1 = params.low_par1->getValue(layer, input.num_layers);
    double low_p2 = params.low_par2->getValue(layer, input.num_layers);
    double low_p3 = params.low_par3->getValue(layer, input.num_layers);
    double high_p1 = params.high_par1->getValue(layer, input.num_layers);
    double high_p2 = params.high_par2->getValue(layer, input.num_layers);
    double high_p3 = params.high_par3->getValue(layer, input.num_layers);
    double in_gamma = params.input_gamma->getValue(layer, input.num_layers);
    double out_gamma = params.output_gamma->getValue(layer, input.num_layers);
    if (params.color_model == Colors::RGB) {
      if (in_gamma != 1.0) {
        if (out_gamma != 1.0) {
          process<Colors::RGB, true, true>(
              input_layer, output_layer, low_p1, low_p2, low_p3,
              high_p1, high_p2, high_p3, in_gamma, out_gamma);
        } else {
          process<Colors::RGB, true, false>(
              input_layer, output_layer, low_p1, low_p2, low_p3,
              high_p1, high_p2, high_p3, in_gamma, out_gamma);
        }
      } else {
        if (out_gamma != 1.0) {
          process<Colors::RGB, false, true>(
              input_layer, output_layer, low_p1, low_p2, low_p3,
              high_p1, high_p2, high_p3, in_gamma, out_gamma);

        } else {
          process<Colors::RGB, false, false>(
              input_layer, output_layer, low_p1, low_p2, low_p3,
              high_p1, high_p2, high_p3, in_gamma, out_gamma);
        }
      }
    } else {
      if (in_gamma != 1.0) {
        if (out_gamma != 1.0) {
          process<Colors::HSV, true, true>(
              input_layer, output_layer, low_p1, low_p2, low_p3,
              high_p1, high_p2, high_p3, in_gamma, out_gamma);
        } else {
          process<Colors::HSV, true, false>(
              input_layer, output_layer, low_p1, low_p2, low_p3,
              high_p1, high_p2, high_p3, in_gamma, out_gamma);
        }
      } else {
        if (out_gamma != 1.0) {
          process<Colors::HSV, false, true>(
              input_layer, output_layer, low_p1, low_p2, low_p3,
              high_p1, high_p2, high_p3, in_gamma, out_gamma);

        } else {
          process<Colors::HSV, false, false>(
              input_layer, output_layer, low_p1, low_p2, low_p3,
              high_p1, high_p2, high_p3, in_gamma, out_gamma);
        }
      }
    }
  }
}

IntegerDataRenderer::Params::Params() {
  set_default_RGB();
}

void IntegerDataRenderer::Params::set_default_RGB() {
  low_par1 = SharedPtrParameter::constant(0);
  low_par2 = SharedPtrParameter::constant(0);
  low_par3 = SharedPtrParameter::constant(0);
  high_par1 = SharedPtrParameter::constant(1);
  high_par2 = SharedPtrParameter::constant(1);
  high_par3 = SharedPtrParameter::constant(1);
  input_gamma = SharedPtrParameter::constant(1);
  output_gamma = SharedPtrParameter::constant(1);
  color_model = Colors::RGB;
}

void IntegerDataRenderer::Params::set_default_HSV() {
  low_par1 = SharedPtrParameter::constant(0);
  low_par2 = SharedPtrParameter::constant(0);
  low_par3 = SharedPtrParameter::constant(0);
  high_par1 = SharedPtrParameter::constant(0);
  high_par2 = SharedPtrParameter::constant(0);
  high_par3 = SharedPtrParameter::constant(1);
  input_gamma = SharedPtrParameter::constant(1);
  output_gamma = SharedPtrParameter::constant(1);
  color_model = Colors::HSV;
}
