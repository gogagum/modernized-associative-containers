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

#include <utility>
#include <type_traits>
#include <ranges>

namespace mstd {

template <std::ranges::input_range RangeT>
using __range_key_type = std::remove_const_t<typename std::ranges::range_value_t<RangeT>::first_type>;

template <std::ranges::input_range RangeT>
using __range_mapped_type = typename std::ranges::range_value_t<RangeT>::second_type;

template <std::ranges::input_range RangeT>
using __range_to_alloc_type =
    std::pair<const typename std::ranges::range_value_t<RangeT>::first_type, typename std::ranges::range_value_t<RangeT>::second_type>;

} // namespace mstd

#endif // MSTD_ITERATOR_RANGES_ITERATOR_TRAITS_HPP
