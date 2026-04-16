// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_RANGES_CONTAINER_COMPATIBLE_RANGE_HPP
#define MSTD_RANGES_CONTAINER_COMPATIBLE_RANGE_HPP

#include <concepts>
#include <iterator>
#include <ranges>

namespace mstd {

template <class RangeT, class _Tp>
concept _ContainerCompatibleRange =
    std::ranges::input_range<RangeT> && std::convertible_to<std::ranges::range_reference_t<RangeT>, _Tp>;

template <class IteratorT, class _Tp>
concept _ContainerCompatibleIterator =
    std::input_iterator<IteratorT> &&
    std::convertible_to<std::iter_reference_t<IteratorT>, _Tp>;


} // namespace mstd

#endif // MSTD_RANGES_CONTAINER_COMPATIBLE_RANGE_HPP