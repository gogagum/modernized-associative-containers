//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_DESUGARS_TO_HPP
#define MSTD_TYPE_TRAITS_DESUGARS_TO_HPP

namespace mstd {

// Tags to represent the canonical operations.

// syntactically, the operation is equivalent to calling `a < b`
struct __less_tag {};

// syntactically, the operation is equivalent to calling `a > b`
struct __greater_tag {};

// This class template is used to determine whether an operation "desugars"
// (or boils down) to a given canonical operation.
//
// For example, `std::equal_to<>`, our internal `std::__equal_to` helper and
// `ranges::equal_to` are all just fancy ways of representing a transparent
// equality operation, so they all desugar to `__equal_tag`.
//
// This is useful to optimize some functions in cases where we know e.g. the
// predicate being passed is actually going to call a builtin operator, or has
// some specific semantics.
template <class CanonicalTag, class OperationT, class... ArgsT>
inline const bool desugars_to_v = false;

template <class Tp>
inline const bool desugars_to_v<__less_tag, std::less<>, Tp, Tp> = true;

template <class Tp, class Up>
inline const bool desugars_to_v<__less_tag, std::ranges::less, Tp, Up> = true;

template <class Tp>
inline const bool desugars_to_v<__greater_tag, std::greater<>, Tp, Tp> = true;

template <class Tp, class Up>
inline const bool desugars_to_v<__greater_tag, std::ranges::greater, Tp, Up> = true;

// For the purpose of determining whether something desugars to something else,
// we disregard const and ref qualifiers on the operation itself.
template <class CanonicalTag, class OperationT, class... ArgsT>
inline const bool desugars_to_v<CanonicalTag, OperationT const, ArgsT...> =
    desugars_to_v<CanonicalTag, OperationT, ArgsT...>;
template <class CanonicalTag, class OperationT, class... ArgsT>
inline const bool desugars_to_v<CanonicalTag, OperationT&, ArgsT...> =
    desugars_to_v<CanonicalTag, OperationT, ArgsT...>;
template <class CanonicalTag, class OperationT, class... ArgsT>
inline const bool desugars_to_v<CanonicalTag, OperationT&&, ArgsT...> =
    desugars_to_v<CanonicalTag, OperationT, ArgsT...>;

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_DESUGARS_TO_HPP
