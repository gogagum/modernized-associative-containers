// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_ITERATOR_RANGES_ITERATOR_TRAITS_HPP
#define MSTD_ITERATOR_RANGES_ITERATOR_TRAITS_HPP

#include <detail/config.hpp>

#include <utility>
#include <concepts>
#include <type_traits>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

MSTD_BEGIN_NAMESPACE_STD

template <std::ranges::input_range _Range>
using __range_key_type MSTD_NODEBUG = std::remove_const_t<typename std::ranges::range_value_t<_Range>::first_type>;

template <std::ranges::input_range _Range>
using __range_mapped_type MSTD_NODEBUG = typename std::ranges::range_value_t<_Range>::second_type;

template <std::ranges::input_range _Range>
using __range_to_alloc_type MSTD_NODEBUG =
    std::pair<const typename std::ranges::range_value_t<_Range>::first_type, typename std::ranges::range_value_t<_Range>::second_type>;

MSTD_END_NAMESPACE

#endif // MSTD_ITERATOR_RANGES_ITERATOR_TRAITS_HPP
