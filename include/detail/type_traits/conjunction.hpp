//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_CONJUNCTION_HPP
#define MSTD_TYPE_TRAITS_CONJUNCTION_HPP

#include "detail/config.hpp"

#include <type_traits>

#include "conjunction.hpp"

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

template <class...>
std::false_type __and_helper(...);

// _And always performs lazy evaluation of its arguments.
//
// However, `_And<_Pred...>` itself will evaluate its result immediately (without having to
// be instantiated) since it is an alias, unlike `conjunction<_Pred...>`, which is a struct.
// If you want to defer the evaluation of `_And<_Pred...>` itself, use `_Lazy<_And, _Pred...>`.
template <class... _Pred>
using _And = decltype(mstd::__and_helper<_Pred...>(0));

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_CONJUNCTION_HPP
