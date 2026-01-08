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

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

MSTD_BEGIN_NAMESPACE_STD

#if MSTD_STD_VER >= 14

template <class _Tp, class _Key = void, class = void>
inline const bool __is_transparent_v = false;

template <class _Tp, class _Key>
inline const bool __is_transparent_v<_Tp, _Key, std::void_t<typename _Tp::is_transparent> > = true;

#endif

// Two types are considered transparently comparable if `comparator(key, arg)` is equivalent to `comparator(key,
// <implicit cast to KeyT>(arg))`.
//
// This is different from `__is_transparent_v`, which is only a property of the comparator and doesn't provide
// additional semantic guarantees.
template <class _Comparator, class _KeyT, class _Arg, class = void>
inline const bool __is_transparently_comparable_v = false;

MSTD_END_NAMESPACE

#endif // MSTD_FUNCTIONAL_IS_TRANSPARENT_HPP
