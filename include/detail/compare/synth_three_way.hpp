
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

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

// [expos.only.func]

inline constexpr auto __synth_three_way = []<class _Tp, class _Up>(const _Tp& __t, const _Up& __u)
  requires requires {
    { __t < __u } -> std::convertible_to<bool>;
    { __u < __t } -> std::convertible_to<bool>;
  }
{
  if constexpr (std::three_way_comparable_with<_Tp, _Up>) {
    return __t <=> __u;
  } else {
    if (__t < __u)
      return std::weak_ordering::less;
    if (__u < __t)
      return std::weak_ordering::greater;
    return std::weak_ordering::equivalent;
  }
};

template <class _Tp, class _Up = _Tp>
using __synth_three_way_result =
    decltype(mstd::__synth_three_way(std::declval<_Tp&>(), std::declval<_Up&>()));

} // namespace mstd

#endif // MSTD_COMPARE_SYNTH_THREE_WAY_HPP
