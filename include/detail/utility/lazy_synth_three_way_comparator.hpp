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
#include <detail/utility/default_three_way_comparator.hpp>

// This file implements a LazySynthThreeWayComparator, which tries to build an efficient three way comparison from
// a binary comparator. That is done in multiple steps:
// 1) Check whether the comparator desugars to a less-than operator
//    If that is the case, check whether there exists a specialization of `__default_three_way_comparator`, which
//    can be specialized to implement a three way comparator for the specific types.
// 2) Fall back to doing a lazy less than/greater than comparison

namespace mstd {

template <class CompT, class LhsT, class RhsT>
struct __lazy_compare_result {
  const CompT& __comp_;
  const LhsT& __lhs_;
  const RhsT& __rhs_;

  __lazy_compare_result(const CompT& comp, const LhsT& lhs, const RhsT& rhs)
  : __comp_(comp)
  , __lhs_(lhs)
  , __rhs_(rhs)
  {}

  bool less() const { return __comp_(__lhs_, __rhs_); }
  bool greater() const { return __comp_(__rhs_, __lhs_); }
};

// This class provides three way comparison between LhsT and RhsT as efficiently as possible. This can be specialized if
// a comparator only compares part of the object, potentially allowing an efficient three way comparison between the
// subobjects. The specialization should use the LazySynthThreeWayComparator for the subobjects to achieve this.
template <class CompT, class LhsT, class RhsT>
struct LazySynthThreeWayComparator {
  const CompT& comp_;

  LazySynthThreeWayComparator(const CompT& comp)
  : comp_(comp) {}

  __lazy_compare_result<CompT, LhsT, RhsT>
  operator()(const LhsT& lhs, const RhsT& rhs) const {
    return {comp_, lhs, rhs};
  }
};

struct __eager_compare_result {
  int __res_;

  explicit __eager_compare_result(int res) : __res_(res) {}

  bool less() const { return __res_ < 0; }
  bool greater() const { return __res_ > 0; }
};

template <class CompT, class LhsT, class RhsT>
requires (desugars_to_v<__less_tag, CompT, LhsT, RhsT> && __has_default_three_way_comparator<LhsT, RhsT>::value)
struct LazySynthThreeWayComparator<CompT, LhsT, RhsT> {
  LazySynthThreeWayComparator(const CompT&) {}

  // Same comment as above.
  static __eager_compare_result
  operator()(const LhsT& lhs, const RhsT& rhs) {
    return __eager_compare_result(__default_three_way_comparator<LhsT, RhsT>()(lhs, rhs));
  }
};

template <class CompT, class LhsT, class RhsT>
requires (desugars_to_v<__greater_tag, CompT, LhsT, RhsT> && __has_default_three_way_comparator<LhsT, RhsT>::value)
struct LazySynthThreeWayComparator<CompT, LhsT, RhsT> {
  LazySynthThreeWayComparator(const CompT&) {}

  // Same comment as above.
  static __eager_compare_result
  operator()(const LhsT& lhs, const RhsT& rhs) {
    return __eager_compare_result(-__default_three_way_comparator<LhsT, RhsT>()(lhs, rhs));
  }
};

} // namespace mstd

#endif // MSTD_UTILITY_LAZY_SYNTH_THREE_WAY_COMPARATOR_HPP
