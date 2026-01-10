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

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

// This struct can be specialized to provide a three way comparator between _LHS and _RHS.
// The return value should be
// - less than zero if (lhs_val < rhs_val)
// - greater than zero if (rhs_val < lhs_val)
// - zero otherwise
template <class _LHS, class _RHS, class = void>
struct __default_three_way_comparator;

template <class _LHS, class _RHS>
struct __default_three_way_comparator<_LHS,
                                      _RHS,
                                      std::enable_if_t<std::is_arithmetic<_LHS>::value && std::is_arithmetic<_RHS>::value> > {
  static int operator()(_LHS __lhs, _RHS __rhs) {
    if (__lhs < __rhs)
      return -1;
    if (__lhs > __rhs)
      return 1;
    return 0;
  }
};

#if __has_builtin(__builtin_lt_synthesizes_from_spaceship)
template <class _LHS, class _RHS>
struct __default_three_way_comparator<
    _LHS,
    _RHS,
    __enable_if_t<!(is_arithmetic<_LHS>::value && is_arithmetic<_RHS>::value) &&
                  __builtin_lt_synthesizes_from_spaceship(const _LHS&, const _RHS&)>> {
  static int operator()(const _LHS& __lhs, const _RHS& __rhs) {
    auto __res = __lhs <=> __rhs;
    if (__res < 0)
      return -1;
    if (__res > 0)
      return 1;
    return 0;
  }
};
#endif

template <class _LHS, class _RHS, bool = true>
struct __has_default_three_way_comparator : std::false_type {};

template <class _LHS, class _RHS>
struct __has_default_three_way_comparator<_LHS, _RHS, sizeof(__default_three_way_comparator<_LHS, _RHS>) >= 0>
    : std::true_type {};

} // namespace mstd

#endif // MSTD_UTILITY_DEFAULT_THREE_WAY_COMPARATOR_HPP
