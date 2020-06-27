//
// Created by rutio on 2020-05-30.
//

#ifndef LUCIMALIR_REGISTERPNGFILENAMES_H
#define LUCIMALIR_REGISTERPNGFILENAMES_H

#include <sstream>
#include <vector>
#include <map>
#include <mutex>

class PngOutput {
public:
  static std::vector<std::string> registerFilenames(const std::string& base, int num_layers);

  static void saveFile(const std::string& filename, int w, int h, void* data);
private:
  static PngOutput& getInstance() {
    static PngOutput instance;
    return instance;
  }

  static std::string getFilename(const std::string& base, int layer, int index);

  std::vector<std::string> _registerFilenames(const std::string& base, int num_layers);

  std::map<std::string, int> next_index;

  std::mutex register_mutex;
};


#endif //LUCIMALIR_REGISTERPNGFILENAMES_H
