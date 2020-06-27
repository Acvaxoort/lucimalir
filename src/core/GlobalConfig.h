//
// Created by rutio on 2020-04-09.
//

#ifndef LUCIMALIR_GLOBALCONFIG_H
#define LUCIMALIR_GLOBALCONFIG_H


#include <cstdint>

class GlobalConfig {
public:
  static GlobalConfig& getInstance();

  uint32_t default_buffer_history = 1;

  bool filter_auto_request = false;

  GlobalConfig(const GlobalConfig&) = delete;
  GlobalConfig& operator=(const GlobalConfig&) = delete;
private:
  GlobalConfig() = default;
};


#endif //LUCIMALIR_GLOBALCONFIG_H
