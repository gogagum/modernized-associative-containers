//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_IS_REFERENCEABLE_HPP
#define MSTD_TYPE_TRAITS_IS_REFERENCEABLE_HPP

#include <detail/config.hpp>

#include <type_traits>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

MSTD_BEGIN_NAMESPACE_STD

template <class _Tp, class = void>
inline const bool __is_referenceable_v = false;

template <class _Tp>
inline const bool __is_referenceable_v<_Tp, std::void_t<_Tp&> > = true;

template <class _Tp>
concept __referenceable = __is_referenceable_v<_Tp>;

MSTD_END_NAMESPACE

#endif // MSTD_TYPE_TRAITS_IS_REFERENCEABLE_HPP
