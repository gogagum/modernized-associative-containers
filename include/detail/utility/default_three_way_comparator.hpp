//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_UTILITY_DEFAULT_THREE_WAY_COMPARATOR_HPP
#define MSTD_UTILITY_DEFAULT_THREE_WAY_COMPARATOR_HPP

#include <detail/config.hpp>
#include <type_traits>

namespace mstd {

// This struct can be specialized to provide a three way comparator between LhsT and RhsT.
// The return value should be
// - less than zero if (lhs_val < rhs_val)
// - greater than zero if (rhs_val < lhs_val)
// - zero otherwise
template <class LhsT, class RhsT>
struct __default_three_way_comparator;

template <class LhsT, class RhsT>
requires (std::is_arithmetic<LhsT>::value && std::is_arithmetic<RhsT>::value)
struct __default_three_way_comparator<LhsT, RhsT> {
  static int operator()(LhsT __lhs, RhsT __rhs) {
    if (__lhs < __rhs)
      return -1;
    if (__lhs > __rhs)
      return 1;
    return 0;
  }
};

#if __has_builtin(__builtin_lt_synthesizes_from_spaceship)
template <class LhsT, class RhsT>
requires (
  !(is_arithmetic<LhsT>::value && is_arithmetic<RhsT>::value)
  && __builtin_lt_synthesizes_from_spaceship(const LhsT&, const _RHS&)
)
struct __default_three_way_comparator<LhsT, RhsT> {
  static int operator()(const LhsT& lhs, const RhsT& rhs) {
    auto res = lhs <=> rhs;
    if (res < 0)
      return -1;
    if (res > 0)
      return 1;
    return 0;
  }
};
#endif

template <class LhsT, class RhsT, bool = true>
struct __has_default_three_way_comparator : std::false_type {};

template <class LhsT, class RhsT>
struct __has_default_three_way_comparator<LhsT, RhsT, sizeof(__default_three_way_comparator<LhsT, RhsT>) >= 0>
    : std::true_type {};

} // namespace mstd

#endif // MSTD_UTILITY_DEFAULT_THREE_WAY_COMPARATOR_HPP
