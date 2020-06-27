//
// Created by rutio on 2020-05-30.
//

#ifndef LUCIMALIR_COLORS_H
#define LUCIMALIR_COLORS_H

#include "DataTypes.h"

namespace Colors {

  const double MAX_VALUE = 255.999;

  enum ColorModel {
    RGB, HSV
  };

  template<class T>
  inline uint8_t limit(T value) {
    return std::max(std::min((int) value, 255), 0);
  }

  template<class T, class U = uint8_t>
  inline U tanh(T value) {
    return std::max(255.0 * std::tanh(value / 255.0), 0.0);
  }

  inline uint8_t byte(double v) {
    return (uint8_t) (v * MAX_VALUE);
  }

  inline uint8_t byte_gamma(double v, double gamma) {
    return (uint8_t) (MAX_VALUE * std::pow(v, gamma));
  }

  inline DataTypes::RGBA convertRGB(double r, double g, double b) {
    DataTypes::RGBA result;
    result.r = byte(r);
    result.g = byte(g);
    result.b = byte(b);
    result.a = 255;
    return result;
  }

  inline DataTypes::RGBA
  convertRGB(double r, double g, double b, double gamma) {
    DataTypes::RGBA result;
    result.r = byte_gamma(r, gamma);
    result.g = byte_gamma(g, gamma);
    result.b = byte_gamma(b, gamma);
    result.a = 255;
    return result;
  }

  inline DataTypes::RGBA convertHSV(double h, double s, double v) {
    DataTypes::RGBA result;
    v *= MAX_VALUE;
    result.a = 255;
    if (s <= 0.0) {
      result.r = (uint8_t) v;
      result.g = (uint8_t) v;
      result.b = (uint8_t) v;
      return result;
    }
    if (h > M_PI * 2.0 || h < 0.0) {
      h -= std::floor(h / (M_PI * 2.0)) * (M_PI * 2.0);
    }
    h *= 3.0 / M_PI;
    int i = (int) h;
    double ff = h - i;
    double p = v * (1.0 - s);
    double q = v * (1.0 - (s * ff));
    double t = v * (1.0 - (s * (1.0 - ff)));
    switch (i) {
      case 0:
        result.r = (uint8_t) v;
        result.g = (uint8_t) t;
        result.b = (uint8_t) p;
        break;
      case 1:
        result.r = (uint8_t) q;
        result.g = (uint8_t) v;
        result.b = (uint8_t) p;
        break;
      case 2:
        result.r = (uint8_t) p;
        result.g = (uint8_t) v;
        result.b = (uint8_t) t;
        break;
      case 3:
        result.r = (uint8_t) p;
        result.g = (uint8_t) q;
        result.b = (uint8_t) v;
        break;
      case 4:
        result.r = (uint8_t) t;
        result.g = (uint8_t) p;
        result.b = (uint8_t) v;
        break;
      case 5:
        result.r = (uint8_t) v;
        result.g = (uint8_t) p;
        result.b = (uint8_t) q;
        break;
    }
    return result;
  }


  inline DataTypes::RGBA
  convertHSV(double h, double s, double v, double gamma) {
    DataTypes::RGBA result;
    result.a = 255;
    if (s <= 0.0) {
      uint8_t vu = byte_gamma(v, gamma);
      result.r = vu;
      result.g = vu;
      result.b = vu;
      return result;
    }
    if (h > M_PI * 2.0 || h < 0.0) {
      h -= std::floor(h / (M_PI * 2.0)) * (M_PI * 2.0);
    }
    h *= 3.0 / M_PI;
    int i = (int) h;
    double ff = h - i;
    double p = v * (1.0 - s);
    double q = v * (1.0 - (s * ff));
    double t = v * (1.0 - (s * (1.0 - ff)));
    switch (i) {
      case 0:
        result.r = byte_gamma(v, gamma);
        result.g = byte_gamma(t, gamma);
        result.b = byte_gamma(p, gamma);
        break;
      case 1:
        result.r = byte_gamma(q, gamma);
        result.g = byte_gamma(v, gamma);
        result.b = byte_gamma(p, gamma);
        break;
      case 2:
        result.r = byte_gamma(p, gamma);
        result.g = byte_gamma(v, gamma);
        result.b = byte_gamma(t, gamma);
        break;
      case 3:
        result.r = byte_gamma(p, gamma);
        result.g = byte_gamma(q, gamma);
        result.b = byte_gamma(v, gamma);
        break;
      case 4:
        result.r = byte_gamma(t, gamma);
        result.g = byte_gamma(p, gamma);
        result.b = byte_gamma(v, gamma);
        break;
      case 5:
        result.r = byte_gamma(v, gamma);
        result.g = byte_gamma(p, gamma);
        result.b = byte_gamma(q, gamma);
        break;
    }
    return result;
  }

}


#endif //LUCIMALIR_COLORS_H
