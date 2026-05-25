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

#include <tuple>
#include <type_traits>

#include <detail/map_value.hpp>

namespace mstd {

template <class IteratorT>
using __iter_key_type = std::remove_const_t<
    std::tuple_element_t<0, std::iter_value_t<IteratorT>>>;

template <class IteratorT>
using __iter_mapped_type
    = std::tuple_element_t<1, std::iter_value_t<IteratorT>>;

template <class IteratorT>
using __iter_to_alloc_type = MapValue<
    std::remove_const_t<std::tuple_element_t<0, std::iter_value_t<IteratorT>>>,
    std::tuple_element_t<1, std::iter_value_t<IteratorT>>>;

} // namespace mstd

#endif // MSTD_ITERATOR_ITERATOR_TRAITS_HPP
