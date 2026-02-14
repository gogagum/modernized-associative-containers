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
#include <detail/config.hpp>
#include <detail/type_traits/nat.hpp>
#include <detail/type_traits/detected_or.hpp>
#include <cstddef>
#include <utility>
#include <tuple>
#include <type_traits>

namespace mstd {

template <class _Tp>
using __iterator_category = typename _Tp::iterator_category;

template <class _Tp, class _Up>
using __has_iterator_category_convertible_to =
    std::is_convertible<__detected_or_t<__nat, __iterator_category, std::iterator_traits<_Tp> >, _Up>;

template <class _Tp>
using __has_input_iterator_category = __has_iterator_category_convertible_to<_Tp, std::input_iterator_tag>;

template <class InputIteratorT>
using __iterator_value_type = typename std::iterator_traits<InputIteratorT>::value_type;

template <class InputIteratorT>
using __iter_key_type = std::remove_const_t<std::tuple_element_t<0, __iterator_value_type<InputIteratorT>>>;

template <class InputIteratorT>
using __iter_mapped_type = std::tuple_element_t<1, __iterator_value_type<InputIteratorT>>;

template <class InputIteratorT>
using __iter_to_alloc_type
  = std::pair<
      const std::tuple_element_t<0, __iterator_value_type<InputIteratorT>>
    , std::tuple_element_t<1, __iterator_value_type<InputIteratorT>>
    >;

} // namespace mstd

#endif // MSTD_ITERATOR_ITERATOR_TRAITS_HPP
