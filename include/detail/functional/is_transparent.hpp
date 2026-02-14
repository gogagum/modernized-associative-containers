// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_FUNCTIONAL_IS_TRANSPARENT_HPP
#define MSTD_FUNCTIONAL_IS_TRANSPARENT_HPP

#include <detail/config.hpp>

#include <type_traits>

namespace mstd {

template <class _Tp, class KeyT = void, class = void>
inline const bool __is_transparent_v = false;

template <class _Tp, class KeyT>
inline const bool __is_transparent_v<_Tp, KeyT, std::void_t<typename _Tp::is_transparent> > = true;

// Two types are considered transparently comparable if `comparator(key, arg)` is equivalent to `comparator(key,
// <implicit cast to KeyT>(arg))`.
//
// This is different from `__is_transparent_v`, which is only a property of the comparator and doesn't provide
// additional semantic guarantees.
template <class CompT, class _KeyT, class ArgT, class = void>
inline const bool __is_transparently_comparable_v = false;

} // namespace mstd

#endif // MSTD_FUNCTIONAL_IS_TRANSPARENT_HPP
