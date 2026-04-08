//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_UTILITY_LAZY_SYNTH_THREE_WAY_COMPARATOR_HPP
#define MSTD_UTILITY_LAZY_SYNTH_THREE_WAY_COMPARATOR_HPP

#include <type_traits>
#include <detail/type_traits/desugars_to.hpp>

// This file implements a LazySynthThreeWayComparator, which tries to build an efficient three way comparison from
// a binary comparator. That is done in multiple steps:
// 1) Check whether the comparator desugars to a less-than operator
//    If that is the case, check whether there exists a specialization of `__default_three_way_comparator`, which
//    can be specialized to implement a three way comparator for the specific types.
// 2) Fall back to doing a lazy less than/greater than comparison

namespace mstd {

  // This struct can be specialized to provide a three way comparator between LhsT and RhsT.
  // The return value should be
  // - less than zero if (lhs_val < rhs_val)
  // - greater than zero if (rhs_val < lhs_val)
  // - zero otherwise
  template <class LhsT, class RhsT>
  struct __default_three_way_comparator;

  template <class LhsT, class RhsT>
    requires(std::is_arithmetic<LhsT>::value && std::is_arithmetic<RhsT>::value)
  struct __default_three_way_comparator<LhsT, RhsT>
  {
    static int operator()(LhsT __lhs, RhsT __rhs)
    {
      if (__lhs < __rhs)
        return -1;
      if (__lhs > __rhs)
        return 1;
      return 0;
    }
  };

#if __has_builtin(__builtin_lt_synthesizes_from_spaceship)
  template <class LhsT, class RhsT>
    requires(
        !(is_arithmetic<LhsT>::value && is_arithmetic<RhsT>::value) && __builtin_lt_synthesizes_from_spaceship(const LhsT &, const RhsT &))
  struct __default_three_way_comparator<LhsT, RhsT>
  {
    static int operator()(const LhsT &lhs, const RhsT &rhs)
    {
      auto res = lhs <=> rhs;
      if (res < 0)
        return -1;
      if (res > 0)
        return 1;
      return 0;
    }
  };
#endif

template <class LhsT, class RhsT>
constexpr static bool has_default_three_way_comparator_v = requires(const LhsT &lhs, const RhsT &rhs) { __default_three_way_comparator<LhsT, RhsT>{}(lhs, rhs); };


// This class provides three way comparison between LhsT and RhsT as efficiently as possible. This can be specialized if
// a comparator only compares part of the object, potentially allowing an efficient three way comparison between the
// subobjects. The specialization should use the LazySynthThreeWayComparator for the subobjects to achieve this.
template <class CompT, class LhsT, class RhsT>
struct LazySynthThreeWayComparator {
  const CompT& comp_;

  struct CompareResult {
    const CompT& comp_;
    const LhsT& lhs_;
    const RhsT& rhs_;

    CompareResult(const CompT& comp, const LhsT& lhs, const RhsT& rhs)
    : comp_(comp)
    , lhs_(lhs)
    , rhs_(rhs)
    {}

    bool less() const { return comp_(lhs_, rhs_); }
    bool greater() const { return comp_(rhs_, lhs_); }
  };

  LazySynthThreeWayComparator(const CompT& comp)
  : comp_(comp) {}

  CompareResult operator()(const LhsT& lhs, const RhsT& rhs) const {
    return {comp_, lhs, rhs};
  }
};

template <class CompT, class LhsT, class RhsT>
requires (
  (desugars_to_v<__less_tag, CompT, LhsT, RhsT> || desugars_to_v<__greater_tag, CompT, LhsT, RhsT>)
  && has_default_three_way_comparator_v<LhsT, RhsT>
)
struct LazySynthThreeWayComparator<CompT, LhsT, RhsT> {
  LazySynthThreeWayComparator(const CompT&) {}

  struct CompareResult {
    int res_;

    CompareResult(int res) : res_(res) {}

    bool less() const { return res_ < 0; }
    bool greater() const { return res_ > 0; }
  };

  // Same comment as above.
  CompareResult operator()(const LhsT& lhs, const RhsT& rhs) {
    const auto cmp_res = __default_three_way_comparator<LhsT, RhsT>()(lhs, rhs);
    if constexpr (desugars_to_v<__less_tag, CompT, LhsT, RhsT>) {
      return cmp_res;
    } else {
      return -cmp_res;
    }
  }
};

} // namespace mstd

#endif // MSTD_UTILITY_LAZY_SYNTH_THREE_WAY_COMPARATOR_HPP
