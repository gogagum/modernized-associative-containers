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
#include <type_traits>
#include <utility>

namespace mstd {

template <class _Alloc, class = void, class = void>
inline const bool __is_allocator_v = false;

template <class _Alloc>
inline const bool __is_allocator_v<_Alloc,
                                   std::void_t<typename _Alloc::value_type>,
                                   std::void_t<decltype(std::declval<_Alloc&>().allocate(size_t()))> > = true;

} // namespace mstd

#endif // MSTD_TYPE_IS_ALLOCATOR_HPP
