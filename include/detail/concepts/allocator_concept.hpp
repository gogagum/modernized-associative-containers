//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_IS_ALLOCATOR_HPP
#define MSTD_TYPE_IS_ALLOCATOR_HPP

#include <cstddef>

namespace mstd {

template <class T>
concept Allocator = requires (T& alloc, std::size_t alloc_size) { alloc.allocate(alloc_size); typename T::value_type; };


} // namespace mstd

#endif // MSTD_TYPE_IS_ALLOCATOR_HPP
