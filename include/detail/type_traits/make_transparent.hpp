//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_MAKE_TRANSPARENT_HPP
#define MSTD_TYPE_TRAITS_MAKE_TRANSPARENT_HPP

#include <detail/config.hpp>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

MSTD_BEGIN_NAMESPACE_STD

// __make_transparent tries to create a transparent comparator from its non-transparent counterpart, e.g. obtain
// `less<>` from `less<T>`. This is useful in cases where conversions can be avoided (e.g. a string literal to a
// std::string).

template <class _Comparator>
struct __make_transparent {
  using type MSTD_NODEBUG = _Comparator;
};

template <class _Comparator>
using __make_transparent_t MSTD_NODEBUG = typename __make_transparent<_Comparator>::type;

template <class _Comparator, std::enable_if_t<std::is_same<_Comparator, __make_transparent_t<_Comparator> >::value, int> = 0>
MSTD_HIDE_FROM_ABI _Comparator& __as_transparent(_Comparator& __comp) {
  return __comp;
}

template <class _Comparator, std::enable_if_t<!std::is_same<_Comparator, __make_transparent_t<_Comparator> >::value, int> = 0>
MSTD_HIDE_FROM_ABI __make_transparent_t<_Comparator> __as_transparent(_Comparator&) {
  static_assert(std::is_empty<_Comparator>::value);
  return __make_transparent_t<_Comparator>();
}

MSTD_END_NAMESPACE

#endif // MSTD_TYPE_TRAITS_MAKE_TRANSPARENT_HPP
