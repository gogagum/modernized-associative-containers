
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_COMPARE_SYNTH_THREE_WAY_HPP
#define MSTD_COMPARE_SYNTH_THREE_WAY_HPP

#include <compare>
#include <concepts>
#include <utility>

#include <detail/config.hpp>

namespace mstd {

// [expos.only.func]

inline constexpr auto __synth_three_way = []<class ValueT, class _Up>(const ValueT& __t, const _Up& __u)
  requires requires {
    { __t < __u } -> std::convertible_to<bool>;
    { __u < __t } -> std::convertible_to<bool>;
  }
{
  if constexpr (std::three_way_comparable_with<ValueT, _Up>) {
    return __t <=> __u;
  } else {
    if (__t < __u)
      return std::weak_ordering::less;
    if (__u < __t)
      return std::weak_ordering::greater;
    return std::weak_ordering::equivalent;
  }
};

template <class ValueT, class _Up = ValueT>
using __synth_three_way_result =
    decltype(mstd::__synth_three_way(std::declval<ValueT&>(), std::declval<_Up&>()));

} // namespace mstd

#endif // MSTD_COMPARE_SYNTH_THREE_WAY_HPP
