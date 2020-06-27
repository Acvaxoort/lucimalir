//
// Created by rutio on 2020-05-30.
//

#include <core/util/Colors.h>
#include "ImageMerge.h"

std::unique_ptr<Filter> ImageMerge::clone() const {
  return std::make_unique<ImageMerge>(*this);
}

const char* ImageMerge::getTypeName() const {
  return "images.merge";
}

const char* ImageMerge::getLabel() const {
  return "Image merge";
}

NonContextualMergeFilterJob<DataTypes::RGBA, DataTypes::RGBA>*
ImageMerge::makeNextJobNonContextualMerge(
    std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output) {
  auto ret = new Job(std::move(input), std::move(output));
  ret->params = params;
  return ret;
}

bool ImageMerge::hasNumberParameter(const std::string& key) {
  FILTER_HAS_ALL_DIMENSIONS() //
  FILTER_HAS(post_multiplier) //
  FILTER_HAS_END()
}

bool ImageMerge::hasStringParameter(const std::string& key) {
  FILTER_HAS(mode) //
  FILTER_HAS_END()
}

bool ImageMerge::hasParameter(const std::string& key) {
  FILTER_HAS(multiplier) //
  FILTER_HAS_END()
}

double ImageMerge::getNumberParameter(const std::string& key) {
  FILTER_GET_ALL_DIMENSIONS() //
  FILTER_GET(post_multiplier) //
  FILTER_GET_END_NUM()
}

std::string ImageMerge::getStringParameter(const std::string& key) {
  FILTER_GET_ENUM(mode) //
    FILTER_GET_ENUM_VALUE("ADD_LIMIT", ADD_LIMIT) //
    FILTER_GET_ENUM_VALUE("ADD_TANH", ADD_TANH) //
    FILTER_GET_ENUM_VALUE("ADD_TANH_LIMIT", ADD_TANH_LIMIT) //
    FILTER_GET_ENUM_VALUE("SUB_LIMIT", SUB_LIMIT) //
    FILTER_GET_ENUM_VALUE("SUB_TANH", SUB_TANH) //
    FILTER_GET_ENUM_VALUE("SUB_TANH_LIMIT", SUB_TANH_LIMIT) //
    FILTER_GET_ENUM_VALUE("MULTIPLY", MULTIPLY) //
    FILTER_GET_ENUM_VALUE("MULTIPLY_SUB", MULTIPLY_SUB) //
  FILTER_GET_ENUM_END("") //
  FILTER_GET_END_STRING()
}

SharedPtrParameter ImageMerge::getParameter(const std::string& key) {
  FILTER_GET(multiplier) FILTER_GET_END_PAR()
}

Filter::ParamChangeStatus
ImageMerge::setNumberParameter(const std::string& key, double value) {
  FILTER_SET_ALL_DIMENSIONS() //
  FILTER_SET(post_multiplier, false) //
  FILTER_SET_END()
}

Filter::ParamChangeStatus ImageMerge::setStringParameter(
    const std::string& key, const std::string& value) {
  FILTER_SET_ENUM(mode, false) //
    FILTER_SET_ENUM_VALUE("ADD_LIMIT", ADD_LIMIT) //
    FILTER_SET_ENUM_VALUE("ADD_TANH", ADD_TANH) //
    FILTER_SET_ENUM_VALUE("ADD_TANH_LIMIT", ADD_TANH_LIMIT) //
    FILTER_SET_ENUM_VALUE("SUB_LIMIT", SUB_LIMIT) //
    FILTER_SET_ENUM_VALUE("SUB_TANH", SUB_TANH) //
    FILTER_SET_ENUM_VALUE("SUB_TANH_LIMIT", SUB_TANH_LIMIT) //
    FILTER_SET_ENUM_VALUE("MULTIPLY", MULTIPLY) //
    FILTER_SET_ENUM_VALUE("MULTIPLY_SUB", MULTIPLY_SUB) //
  FILTER_SET_ENUM_END("invalid value, see help.params.images.merge") //
  FILTER_SET_END()
}

Filter::ParamChangeStatus
ImageMerge::setParameter(const std::string& key, SharedPtrParameter value) {
  FILTER_SET(multiplier, false) //
  FILTER_SET_END()
}

void ImageMerge::Job::completeNonContextualMerge(
    MatrixLayerArrayView<DataTypes::RGBA> input,
    MatrixLayerArrayView<DataTypes::RGBA> output) {
  auto output_layer = output.getLayerView(0);
  std::vector<double> layer_mul(input.num_layers);
  for (int i = 0; i < input.num_layers; ++i) {
    layer_mul[i] = params.multiplier->getValue(i, input.num_layers);
  }
  switch (params.mode) {
    case ADD_LIMIT: {
      for (int i = 0; i < output_layer.data_length; ++i) {
        double acc_r = 0;
        double acc_g = 0;
        double acc_b = 0;
        for (int j = 0; j < input.num_layers; ++j) {
          DataTypes::RGBA col = input.at(j, i);
          acc_r += col.r * layer_mul[j];
          acc_g += col.g * layer_mul[j];
          acc_b += col.b * layer_mul[j];
        }
        output_layer[i].r = Colors::limit(acc_r);
        output_layer[i].g = Colors::limit(acc_g);
        output_layer[i].b = Colors::limit(acc_b);
        output_layer[i].a = 255;
      }
    }
      break;
    case ADD_TANH: {
      for (int i = 0; i < output_layer.data_length; ++i) {
        double acc_r = 0;
        double acc_g = 0;
        double acc_b = 0;
        for (int j = 0; j < input.num_layers; ++j) {
          DataTypes::RGBA col = input.at(j, i);
          acc_r += col.r * layer_mul[j];
          acc_g += col.g * layer_mul[j];
          acc_b += col.b * layer_mul[j];
        }
        output_layer[i].r = Colors::tanh(acc_r);
        output_layer[i].g = Colors::tanh(acc_g);
        output_layer[i].b = Colors::tanh(acc_b);
        output_layer[i].a = 255;
      }
    }
      break;
    case ADD_TANH_LIMIT: {
      for (int i = 0; i < output_layer.data_length; ++i) {
        double acc_r = 0;
        double acc_g = 0;
        double acc_b = 0;
        for (int j = 0; j < input.num_layers; ++j) {
          DataTypes::RGBA col = input.at(j, i);
          acc_r += col.r * layer_mul[j];
          acc_g += col.g * layer_mul[j];
          acc_b += col.b * layer_mul[j];
        }
        acc_r = Colors::tanh<double, double>(acc_r) * params.post_multiplier;
        acc_g = Colors::tanh<double, double>(acc_g) * params.post_multiplier;
        acc_b = Colors::tanh<double, double>(acc_b) * params.post_multiplier;
        output_layer[i].r = Colors::limit(acc_r);
        output_layer[i].g = Colors::limit(acc_g);
        output_layer[i].b = Colors::limit(acc_b);
        output_layer[i].a = 255;
      }
    }
      break;
    case SUB_LIMIT: {
      for (int i = 0; i < output_layer.data_length; ++i) {
        double acc_r = 0;
        double acc_g = 0;
        double acc_b = 0;
        for (int j = 0; j < input.num_layers; ++j) {
          DataTypes::RGBA col = input.at(j, i);
          acc_r += (255 - col.r) * layer_mul[j];
          acc_g += (255 - col.g) * layer_mul[j];
          acc_b += (255 - col.b) * layer_mul[j];
        }
        output_layer[i].r = 255 - Colors::limit(acc_r);
        output_layer[i].g = 255 - Colors::limit(acc_g);
        output_layer[i].b = 255 - Colors::limit(acc_b);
        output_layer[i].a = 255;
      }
    }
      break;
    case SUB_TANH: {
      for (int i = 0; i < output_layer.data_length; ++i) {
        double acc_r = 0;
        double acc_g = 0;
        double acc_b = 0;
        for (int j = 0; j < input.num_layers; ++j) {
          DataTypes::RGBA col = input.at(j, i);
          acc_r += (255 - col.r) * layer_mul[j];
          acc_g += (255 - col.g) * layer_mul[j];
          acc_b += (255 - col.b) * layer_mul[j];
        }
        output_layer[i].r = 255 - Colors::tanh(acc_r);
        output_layer[i].g = 255 - Colors::tanh(acc_g);
        output_layer[i].b = 255 - Colors::tanh(acc_b);
        output_layer[i].a = 255;
      }
    }
      break;
    case SUB_TANH_LIMIT: {
      for (int i = 0; i < output_layer.data_length; ++i) {
        double acc_r = 0;
        double acc_g = 0;
        double acc_b = 0;
        for (int j = 0; j < input.num_layers; ++j) {
          DataTypes::RGBA col = input.at(j, i);
          acc_r += (255 - col.r) * layer_mul[j];
          acc_g += (255 - col.g) * layer_mul[j];
          acc_b += (255 - col.b) * layer_mul[j];
        }
        acc_r = Colors::tanh<double, double>(acc_r) * params.post_multiplier;
        acc_g = Colors::tanh<double, double>(acc_g) * params.post_multiplier;
        acc_b = Colors::tanh<double, double>(acc_b) * params.post_multiplier;
        output_layer[i].r = 255 - Colors::limit(acc_r);
        output_layer[i].g = 255 - Colors::limit(acc_g);
        output_layer[i].b = 255 - Colors::limit(acc_b);
        output_layer[i].a = 255;
      }
    }
      break;
    case MULTIPLY: {
      for (int i = 0; i < output_layer.data_length; ++i) {
        double acc_r = 1.0;
        double acc_g = 1.0;
        double acc_b = 1.0;
        for (int j = 0; j < input.num_layers; ++j) {
          DataTypes::RGBA col = input.at(j, i);
          acc_r *= col.r / 255.0;
          acc_g *= col.g / 255.0;
          acc_b *= col.b / 255.0;
        }
        output_layer[i].r = (uint8_t) (acc_r * 255.0);
        output_layer[i].g = (uint8_t) (acc_g * 255.0);
        output_layer[i].b = (uint8_t) (acc_b * 255.0);
        output_layer[i].a = 255;
      }
    }
      break;
    case MULTIPLY_SUB: {
      for (int i = 0; i < output_layer.data_length; ++i) {
        double acc_r = 1.0;
        double acc_g = 1.0;
        double acc_b = 1.0;
        for (int j = 0; j < input.num_layers; ++j) {
          DataTypes::RGBA col = input.at(j, i);
          acc_r *= (255 - col.r) / 255.0;
          acc_g *= (255 - col.g) / 255.0;
          acc_b *= (255 - col.b) / 255.0;
        }
        output_layer[i].r = 255 - (uint8_t) (acc_r * 255.0);
        output_layer[i].g = 255 - (uint8_t) (acc_g * 255.0);
        output_layer[i].b = 255 - (uint8_t) (acc_b * 255.0);
        output_layer[i].a = 255;
      }
    }
  }
}

ImageMerge::Job::Job(
    std::shared_ptr<Buffer> input, std::shared_ptr<Buffer> output)
    : NonContextualMergeFilterJob<DataTypes::RGBA, DataTypes::RGBA>(
    std::move(input), std::move(output)) {
}
