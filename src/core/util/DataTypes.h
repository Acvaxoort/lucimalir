//
// Created by rutio on 2020-04-07.
//

#ifndef LUCIMALIR_DATATYPES_H
#define LUCIMALIR_DATATYPES_H

#ifdef ENABLE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#include <boost/multiprecision/complex128.hpp>
#endif

#include "RenderingView.h"

namespace DataTypes {

  typedef float FLOAT32;
  typedef std::complex<float> COMPLEX32;
  typedef Vec4D<float> QUATERNION32;
  typedef double FLOAT64;
  typedef std::complex<double> COMPLEX64;
  typedef Vec4D<double> QUATERNION64;
#ifdef ENABLE_FLOAT128
  typedef boost::multiprecision::float128 FLOAT128;
  typedef boost::multiprecision::complex128 COMPLEX128;
  typedef Vec4D<float> QUATERNION128;
#else
  typedef double FLOAT128;
  typedef std::complex<double> COMPLEX128;
  typedef Vec4D<double> QUATERNION128;
#endif
  typedef uint32_t INTEGER;

  enum FloatingPrecision {
    FP32, FP64, FP128
  };
  struct RGBA {
    RGBA(uint32_t c = 0) { auto& p = (uint32_t&)r; p = c; }
    operator uint32_t() { auto& p = (uint32_t&)r; return p; }
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };

  template <typename T>
  struct TypeName {
    //static constexpr const char name[] = "?";
  };

  template <>
  struct TypeName<float> {
    static constexpr const char name[] = "32bit float";
  };

  template <>
  struct TypeName<double> {
    static constexpr const char name[] = "64bit float";
  };

  template <>
  struct TypeName<boost::multiprecision::float128> {
    static constexpr const char name[] = "128bit float";
  };

  template <>
  struct TypeName<Vec4D<float>> {
    static constexpr const char name[] = "32bit float quaternion";
  };

  template <>
  struct TypeName<Vec4D<double>> {
    static constexpr const char name[] = "64bit float quaternion";
  };

  template <>
  struct TypeName<Vec4D<boost::multiprecision::float128>> {
    static constexpr const char name[] = "128bit float quaternion";
  };

  template <>
  struct TypeName<uint32_t> {
    static constexpr const char name[] = "raw integer data";
  };

  template <>
  struct TypeName<RGBA> {
    static constexpr const char name[] = "RGBA";
  };

/*
  template<class T>
  T sin(T x) {
    return std::sin(x);
  }

  template<>
  DataTypes::FLOAT128 sin(DataTypes::FLOAT128 x) {
    return boost::multiprecision::sin(x);
  }

  template<class T>
  T cos(T x) {
    return std::cos(x);
  }

  template<>
  DataTypes::FLOAT128 cos(DataTypes::FLOAT128 x) {
    return boost::multiprecision::cos(x);
  }*/


}

#endif //LUCIMALIR_DATATYPES_H
