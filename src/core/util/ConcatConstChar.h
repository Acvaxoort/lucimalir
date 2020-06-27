//
// Created by rutio on 2020-04-23.
//

#ifndef LUCIMALIR_CONCATCONSTCHAR_H
#define LUCIMALIR_CONCATCONSTCHAR_H

//https://stackoverflow.com/questions/24783400/concatenate-compile-time-strings-in-a-template-at-compile-time

#include <utility>

template<int...I> using is      = std::integer_sequence<int,I...>;
template<int N>   using make_is = std::make_integer_sequence<int,N>;

constexpr auto size(const char*s) { int i = 0; while(*s!=0){++i;++s;} return i; }

template<const char*, typename, const char*, typename>
struct concat_impl;

template<const char* S1, int... I1, const char* S2, int... I2>
struct concat_impl<S1, is<I1...>, S2, is<I2...>> {
  static constexpr const char value[]
      {
          S1[I1]..., S2[I2]..., 0
      };
};

template<const char* S1, const char* S2>
constexpr auto concat {
    concat_impl<S1, make_is<size(S1)>, S2, make_is<size(S2)>>::value
};

#endif //LUCIMALIR_CONCATCONSTCHAR_H
