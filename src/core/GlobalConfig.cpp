//
// Created by rutio on 2020-04-09.
//

#include "GlobalConfig.h"

GlobalConfig& GlobalConfig::getInstance() {
  static GlobalConfig instance;
  return instance;
}
