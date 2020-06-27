//
// Created by rutio on 2020-04-07.
//

#ifndef LUCIMALIR_RENDERINGVIEW_H
#define LUCIMALIR_RENDERINGVIEW_H

#include <cmath>
#include <iomanip>
#include <boost/multiprecision/float128.hpp>

using std::sin;
using boost::multiprecision::sin;
using std::cos;
using boost::multiprecision::cos;

template<typename T>
struct Rot4D {
  static Rot4D XY(T a) {
    T c = cos(a);
    T s = sin(a);
    return {{c, s, 0, 0,
                -s, c, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1}};
  }

  static Rot4D YZ(T a) {
    T c = cos(a);
    T s = sin(a);
    return {{1, 0, 0, 0,
                0, c, s, 0,
                0, -s, c, 0,
                0, 0, 0, 1}};
  }

  static Rot4D ZW(T a) {
    T c = cos(a);
    T s = sin(a);
    return {{1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, c, s,
                0, 0, -s, c}};
  }

  static Rot4D XZ(T a) {
    T c = cos(a);
    T s = sin(a);
    return {{c, 0, -s, 0,
                0, 1, 0, 0,
                s, 0, c, 0,
                0, 0, 0, 1}};
  }

  static Rot4D YW(T a) {
    T c = cos(a);
    T s = sin(a);
    return {{1, 0, 0, 0,
                0, c, 0, -s,
                0, 0, 1, 0,
                0, s, 0, c}};
  }

  static Rot4D XW(T a) {
    T c = cos(a);
    T s = sin(a);
    return {{c, 0, 0, s,
                0, 1, 0, 0,
                0, 0, 1, 0,
                -s, 0, 0, c}};
  }

  inline T& operator[](int i) { return v[i]; }

  inline const T& operator[](int i) const { return v[i]; }

  inline Rot4D<T> operator*(const Rot4D<T>& other) const {
    Rot4D<T> result;
    for (int j = 0; j < 4; ++j) {
      for (int i = 0; i < 4; ++i) {
        result[j * 4 + i] = 0;
        for (int k = 0; k < 4; ++k) {
          result[j * 4 + i] += v[j * 4 + k] * other[k * 4 + i];
        }
      }
    }
    return result;
  }

  inline Rot4D<T>& operator*=(const Rot4D<T>& other) {
    Rot4D<T> result;
    for (int j = 0; j < 4; ++j) {
      for (int i = 0; i < 4; ++i) {
        result[j * 4 + i] = 0;
        for (int k = 0; k < 4; ++k) {
          result[j * 4 + i] += v[j * 4 + k] * other[k * 4 + i];
        }
      }
    }
    *this = result;
    return *this;
  }

  template<typename U>
  inline Rot4D<U> convert() {
    return {{(U) v[0], (U) v[1], (U) v[2], (U) v[3], (U) v[4], (U) v[5], (U) v[6], (U) v[7],
                (U) v[8], (U) v[9], (U) v[10], (U) v[11], (U) v[12], (U) v[13], (U) v[14], (U) v[15]}};
  }

  template<typename U>
  explicit inline operator Rot4D<U>() const {
    return {{(U) v[0], (U) v[1], (U) v[2], (U) v[3], (U) v[4], (U) v[5], (U) v[6], (U) v[7],
                (U) v[8], (U) v[9], (U) v[10], (U) v[11], (U) v[12], (U) v[13], (U) v[14], (U) v[15]}};
  }

  void print(std::ostream& out) {
    out << std::setw(12) << v[0] << "," << std::setw(12) << v[1] << ","
        << std::setw(12) << v[2] << "," << std::setw(12)
        << v[3] << "\n"
        << std::setw(12) << v[4] << "," << std::setw(12) << v[5] << ","
        << std::setw(12) << v[6] << "," << std::setw(12)
        << v[7] << "\n"
        << std::setw(12) << v[8] << "," << std::setw(12) << v[9] << ","
        << std::setw(12) << v[10] << ","
        << std::setw(12) << v[11] << "\n"
        << std::setw(12) << v[12] << "," << std::setw(12) << v[13] << ","
        << std::setw(12) << v[14] << ","
        << std::setw(12) << v[15] << "\n";
  }

  std::array<T, 16> v = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
};

template<typename T>
struct Vec4D {
  Vec4D<T>() = default;

  Vec4D<T>(T x, T y, T z, T w) : v{x, y, z, w} {}

  inline T& operator[](int i) { return v[i]; }

  inline const T& operator[](int i) const { return v[i]; }

  inline Vec4D<T> operator+(const Vec4D<T>& other) const {
    return {v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2],
            v[3] + other.v[3]};
  }

  inline Vec4D<T>& operator+=(const Vec4D<T>& other) {
    v[0] += other.v[0];
    v[1] += other.v[1];
    v[2] += other.v[2];
    v[3] += other.v[3];
    return *this;
  }

  inline Vec4D<T> operator-(const Vec4D<T>& other) const {
    return {v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2],
            v[3] - other.v[3]};
  }

  inline Vec4D<T>& operator-=(const Vec4D<T>& other) {
    v[0] -= other.v[0];
    v[1] -= other.v[1];
    v[2] -= other.v[2];
    v[3] -= other.v[3];
    return *this;
  }

  inline Vec4D<T> operator*(T mul) const {
    return {v[0] * mul, v[1] * mul, v[2] * mul, v[3] * mul};
  }

  inline Vec4D<T>& operator*=(T mul) {
    v[0] *= mul;
    v[1] *= mul;
    v[2] *= mul;
    v[3] *= mul;
    return *this;
  }

  inline Vec4D<T> operator*(const Rot4D<T>& r) const {
    return {
        v[0] * r[0] + v[1] * r[1] + v[2] * r[2] + v[3] * r[3],
        v[0] * r[4] + v[1] * r[5] + v[2] * r[6] + v[3] * r[7],
        v[0] * r[8] + v[1] * r[9] + v[2] * r[10] + v[3] * r[11],
        v[0] * r[12] + v[1] * r[13] + v[2] * r[14] + v[3] * r[15]
    };
  }

  inline Vec4D<T>& operator*=(const Rot4D<T>& r) {
    T new_v[4] = {
        v[0] * r[0] + v[1] * r[1] + v[2] * r[2] + v[3] * r[3],
        v[0] * r[4] + v[1] * r[5] + v[2] * r[6] + v[3] * r[7],
        v[0] * r[8] + v[1] * r[9] + v[2] * r[10] + v[3] * r[11],
        v[0] * r[12] + v[1] * r[13] + v[2] * r[14] + v[3] * r[15]
    };
    v[0] = new_v[0];
    v[1] = new_v[1];
    v[2] = new_v[2];
    v[3] = new_v[3];
    return *this;
  }

  inline Vec4D<T> operator/(T mul) const {
    return {v[0] / mul, v[1] / mul, v[2] / mul, v[3] / mul};
  }

  inline Vec4D<T>& operator/=(T mul) {
    v[0] /= mul;
    v[1] /= mul;
    v[2] /= mul;
    v[3] /= mul;
    return *this;
  }

  template<typename U>
  inline Vec4D<U> convert() const {
    return {(U) v[0], (U) v[1], (U) v[2], (U) v[3]};
  }

  template<typename U>
  explicit inline operator Vec4D<U>() const {
    return {(U) v[0], (U) v[1], (U) v[2], (U) v[3]};
  }

  std::array<T, 4> v = {0, 0, 0, 0};
};

template<typename T>
struct RenderingView {
  Vec4D<T> pos;
  Rot4D<T> rot;
  T scale = 4;

//  template<typename U>
//  inline RenderingView<U> convert() const {
//    return { pos.convert<U>(), (Rot4D<U>) rot, (U) scale };
//  }

  template<typename U>
  explicit inline operator RenderingView<U>() const {
    return {(Vec4D<U>) pos, (Rot4D<U>) rot, (U) scale};
  }


};

template<typename T>
struct RenderingViewCoordinateGenerator {
  RenderingViewCoordinateGenerator(
      const RenderingView<T>& view, int width, int height)
      : begin_pos(view.pos),
        x_increment(view.rot[0], view.rot[1], view.rot[2], view.rot[3]),
        y_increment(-view.rot[4], -view.rot[5], -view.rot[6], -view.rot[7]) {
    T x_mul = 2 * width / (T) (width + height);
    T y_mul = 2 * height / (T) (width + height);
    x_increment *= view.scale * x_mul;
    y_increment *= view.scale * y_mul;
    begin_pos -= x_increment * (T) 0.5;
    begin_pos -= y_increment * (T) 0.5;
    x_increment /= width;
    y_increment /= height;
  }

  Vec4D<T> getCoordinates(int x, int y) {
    return begin_pos + x_increment * x + y_increment * y;
  }

  Vec4D<T> begin_pos;
  Vec4D<T> x_increment;
  Vec4D<T> y_increment;
};

// Wrappers for lua interface
struct PositionWrapper {
  Vec4D<double> pos;

  PositionWrapper() = default;

  PositionWrapper(const Vec4D<double>& pos)
      : pos(pos) {}

  PositionWrapper(const std::vector<double>& vec) {
    if (vec.size() != 4) {
      throw std::invalid_argument("expected length 4");
    }
    std::copy(vec.begin(), vec.end(), pos.v.begin());
  }

  std::vector<double> getValues() const {
    return {pos.v.begin(), pos.v.end()};
  }

  PositionWrapper translate(const PositionWrapper& v) const {
    PositionWrapper ret(*this);
    ret.pos += v.pos;
    return ret;
  }

  std::string tostring() {
    std::stringstream ss;
    ss << "geom4d.pos{";
    for (int i = 0; i < 4; ++i) {
      if (i > 0) {
        ss << ", ";
      }
      ss << pos[i];
    }
    ss << "}";
    return ss.str();
  }

};

struct RotationWrapper {
  Rot4D<double> rot;

  RotationWrapper() = default;

  RotationWrapper(const Rot4D<double>& rot)
      : rot(rot) {}

  RotationWrapper(const std::vector<double>& vec) {
    if (vec.size() != 16) {
      throw std::invalid_argument("expected length 16");
    }
    std::copy(vec.begin(), vec.end(), rot.v.begin());
  }

  static RotationWrapper XY(double a) {
    return Rot4D<double>::XY(a);
  }

  static RotationWrapper YZ(double a) {
    return Rot4D<double>::YZ(a);
  }

  static RotationWrapper ZW(double a) {
    return Rot4D<double>::ZW(a);
  }

  static RotationWrapper XZ(double a) {
    return Rot4D<double>::XZ(a);
  }

  static RotationWrapper YW(double a) {
    return Rot4D<double>::YW(a);
  }

  static RotationWrapper XW(double a) {
    return Rot4D<double>::XW(a);
  }

  std::vector<double> getValues() const {
    return {rot.v.begin(), rot.v.end()};
  }

  RotationWrapper rotate(const RotationWrapper& v) const {
    RotationWrapper ret(*this);
    ret.rot *= v.rot;
    return ret;
  }

  std::string tostring() {
    std::stringstream ss;
    ss << "geom4d.rot{";
    for (int i = 0; i < 16; ++i) {
      if (i > 0) {
        ss << ", ";
      }
      ss << rot[i];
    }
    ss << "}";
    return ss.str();
  }

};

struct ViewWrapper {
  RenderingView<double> view;

  ViewWrapper() = default;

  ViewWrapper(
      const PositionWrapper& pos, const RotationWrapper& rot, double scale)
      : view{pos.pos, rot.rot, scale} {}

  ViewWrapper(const RenderingView<double> view)
      : view(view) {}

  operator RenderingView<double>() const {
    return view;
  }

  PositionWrapper getPosition() const {
    return view.pos;
  }

  RotationWrapper getRotation() const {
    return view.rot;
  }

  double getScale() const {
    return view.scale;
  }

  ViewWrapper setPosition(const PositionWrapper& v) const {
    ViewWrapper ret(*this);
    ret.view.pos = v.pos;
    return ret;
  }

  ViewWrapper setRotation(const RotationWrapper& v) const {
    ViewWrapper ret(*this);
    ret.view.rot = v.rot;
    return ret;
  }

  ViewWrapper setScale(double v) const {
    ViewWrapper ret(*this);
    ret.view.scale = v;
    return ret;
  }

  ViewWrapper translateWithRotationScale(
      const PositionWrapper& v, bool with_rotation, bool with_scale) const {
    ViewWrapper ret(*this);
    if (with_rotation) {
      if (with_scale) {
        ret.view.pos += v.pos * view.rot * view.scale;
      } else {
        ret.view.pos += v.pos * view.rot;
      }
    } else {
      if (with_scale) {
        ret.view.pos += v.pos * view.scale;
      } else {
        ret.view.pos += v.pos;
      }
    }
    return ret;
  }

  ViewWrapper translate(const PositionWrapper& v) const {
    ViewWrapper ret(*this);
    ret.view.pos += v.pos;
    return ret;
  }

  ViewWrapper rotate(const RotationWrapper& v) const {
    ViewWrapper ret(*this);
    ret.view.rot *= v.rot;
    return ret;
  }

  ViewWrapper rescale(double v) const {
    ViewWrapper ret(*this);
    ret.view.scale *= v;
    return ret;
  }

  std::string tostring() {
    std::stringstream ss;
    ss << "geom4d.view(\n  "
       << PositionWrapper(view.pos).tostring() << ",\n  "
       << RotationWrapper(view.rot).tostring() << ",\n  "
       << view.scale << "\n"
       << ")";
    return ss.str();
  }
};


#endif //LUCIMALIR_RENDERINGVIEW_H
