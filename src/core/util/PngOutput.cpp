//
// Created by rutio on 2020-05-30.
//

#include "PngOutput.h"
#include <thread>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

std::vector<std::string> PngOutput::registerFilenames(const std::string& base, int num_layers) {
  return getInstance()._registerFilenames(base, num_layers);
}

void PngOutput::saveFile(const std::string& filename, int w, int h, void* data) {
  stbi_write_png(filename.c_str(), w, h, 4, data, w * 4);
}

std::string PngOutput::getFilename(const std::string& base, int layer, int index) {
  std::stringstream ss;
  ss << base << '_' << layer << '_';
  if (index < 10) {
    ss << '0';
  }
  if (index < 100) {
    ss << '0';
  }
  if (index < 100) {
    ss << '0';
  }
  ss << index << ".png";
  return ss.str();
}

inline bool exists_test1 (const std::string& name) {
  if (FILE *file = fopen(name.c_str(), "r")) {
    fclose(file);
    return true;
  } else {
    return false;
  }
}

std::vector<std::string> PngOutput::_registerFilenames(const std::string& base, int num_layers) {
  std::lock_guard<std::mutex> lock(register_mutex);
  int index;
  auto it = next_index.find(base);
  if (it != next_index.end()) {
    it->second++;
    index = it->second;
  } else {
    index = 0;
    while (exists_test1(getFilename(base, 0, index))) {
      index++;
    }
    next_index.emplace_hint(it, base, index);
  }
  std::vector<std::string> result;
  result.reserve(num_layers);
  for (int i = 0; i < num_layers; ++i) {
    result.push_back(getFilename(base, i, index));
  }
  return result;
}