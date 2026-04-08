// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_ITERATOR_ITERATOR_TRAITS_HPP
#define MSTD_ITERATOR_ITERATOR_TRAITS_HPP

#include <concepts>
#include <cstddef>
#include <utility>
#include <tuple>
#include <type_traits>
#include <concepts>

namespace mstd {

template <class _Tp>
concept __has_input_iterator_category
    = std::convertible_to<typename std::iterator_traits<_Tp>::iterator_category, std::input_iterator_tag>;

template <class InputIteratorT>
using __iter_key_type = std::remove_const_t<std::tuple_element_t<0, std::iter_value_t<InputIteratorT>>>;

template <class InputIteratorT>
using __iter_mapped_type = std::tuple_element_t<1, std::iter_value_t<InputIteratorT>>;

template <class InputIteratorT>
using __iter_to_alloc_type
  = std::pair<
      const std::tuple_element_t<0, std::iter_value_t<InputIteratorT>>
    , std::tuple_element_t<1, std::iter_value_t<InputIteratorT>>
    >;

} // namespace mstd

#endif // MSTD_ITERATOR_ITERATOR_TRAITS_HPP
