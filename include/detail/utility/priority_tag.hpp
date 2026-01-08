//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_UTILITY_PRIORITY_TAG_HPP
#define MSTD_UTILITY_PRIORITY_TAG_HPP

#include <detail/config.hpp>
#include <cstddef>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

MSTD_BEGIN_NAMESPACE_STD

template <size_t _Ip>
struct __priority_tag : __priority_tag<_Ip - 1> {};
template <>
struct __priority_tag<0> {};

MSTD_END_NAMESPACE

#endif // MSTD_UTILITY_PRIORITY_TAG_HPP
