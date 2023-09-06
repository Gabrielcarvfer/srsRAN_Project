/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include <numeric>

namespace srsran {

/// Calculates the greatest common divisor (GCD) of two integers.
template <typename Integer>
Integer gcd(Integer a, Integer b)
{
  while (true) {
    if (a == 0) {
      return b;
    }
    b %= a;
    if (b == 0) {
      return a;
    }
    a %= b;
  }
}

/// Calculates the least common multiplier (LCM) of two integers.
template <typename Integer>
Integer lcm(Integer a, Integer b)
{
  Integer temp = gcd(a, b);

  return temp != 0 ? (a / temp * b) : 0;
}

/// Calculates the least common multiplier (LCM) for a range of integers.
template <typename Integer>
Integer lcm(span<const Integer> values)
{
  return std::accumulate(
      values.begin(), values.end(), Integer(1), [](Integer a, Integer b) { return lcm<Integer>(a, b); });
}

} // namespace srsran