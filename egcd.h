// -*- mode: C++; coding: utf-8; -*-
#ifndef egcd_h
#define egcd_h

#include <tuple>

// Extended Euclidean Algorithm and Modular Inverse, from
// <https://github.com/PetarV-/Algorithms/blob/master/
//   Mathematical Algorithms/Extended Euclidean Algorithm.cpp>

// The MIT License (MIT)

// Copyright (c) 2015 Petar Veličković

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

inline std::pair<std::int64_t, std::pair<std::int64_t, std::int64_t>> egcd(
  std::int64_t a, std::int64_t b) {
  std::int64_t aa = 1, ab = 0, ba = 0, bb = 1;
  while (true) {
    std::int64_t q = a / b;
    if (a == b * q)
      return std::make_pair(b, std::make_pair(ba, bb));
    std::int64_t tmp_a = a;
    std::int64_t tmp_aa = aa;
    std::int64_t tmp_ab = ab;
    a = b;
    b = tmp_a - b * q;
    aa = ba;
    ab = bb;
    ba = tmp_aa - ba * q;
    bb = tmp_ab - bb * q;
  }
}

inline std::int64_t modinv(std::int64_t a, std::int64_t b) {
  std::int64_t b0 = b;
  std::int64_t aa = 1, ba = 0;
  while (true) {
    std::int64_t q = a / b;
    if (a == b * q) {
      if (b != 1) {
        // Modular inverse does not exist!
        return -1;
      }
      while (ba < 0)
        ba += b0;
      return ba;
    }
    std::int64_t tmp_a = a;
    std::int64_t tmp_aa = aa;
    a = b;
    b = tmp_a - b * q;
    aa = ba;
    ba = tmp_aa - ba * q;
  }
}

#endif
