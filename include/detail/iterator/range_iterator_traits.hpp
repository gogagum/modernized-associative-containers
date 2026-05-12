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

#include <tuple>
#include <type_traits>
#include <ranges>

#include <detail/map_value.hpp>

namespace mstd {

template <std::ranges::input_range RangeT>
using __range_key_type
    = std::remove_const_t<
        std::tuple_element_t<0, std::ranges::range_value_t<RangeT>>
      >;

template <std::ranges::input_range RangeT>
using __range_mapped_type
    = std::tuple_element_t<1, std::ranges::range_value_t<RangeT>>;

template <std::ranges::input_range RangeT>
using __range_to_alloc_type
    = MapValue<
        std::remove_const_t<std::tuple_element_t<0, std::ranges::range_value_t<RangeT>>>,
        std::tuple_element_t<1, std::ranges::range_value_t<RangeT>>
      >;

} // namespace mstd

#endif // MSTD_ITERATOR_RANGES_ITERATOR_TRAITS_HPP
