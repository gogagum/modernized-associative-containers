//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_DISJUNCTION_HPP
#define MSTD_TYPE_TRAITS_DISJUNCTION_HPP

#include <detail/config.hpp>
#include <type_traits>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

template <bool>
struct _OrImpl;

template <>
struct _OrImpl<true> {
  template <class _Res, class _First, class... _Rest>
  using _Result MSTD_NODEBUG =
      typename _OrImpl<!bool(_First::value) && sizeof...(_Rest) != 0>::template _Result<_First, _Rest...>;
};

template <>
struct _OrImpl<false> {
  template <class _Res, class...>
  using _Result MSTD_NODEBUG = _Res;
};

// _Or always performs lazy evaluation of its arguments.
//
// However, `_Or<_Pred...>` itself will evaluate its result immediately (without having to
// be instantiated) since it is an alias, unlike `disjunction<_Pred...>`, which is a struct.
// If you want to defer the evaluation of `_Or<_Pred...>` itself, use `_Lazy<_Or, _Pred...>`
// or `disjunction<_Pred...>` directly.
template <class... _Args>
using _Or MSTD_NODEBUG = typename _OrImpl<sizeof...(_Args) != 0>::template _Result<std::false_type, _Args...>;

} // namespace mstd

#endif // MSTD___TYPE_TRAITS_DISJUNCTION_H