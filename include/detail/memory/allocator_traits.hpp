// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_MEMORY_ALLOCATOR_TRAITS_HPP
#define MSTD_MEMORY_ALLOCATOR_TRAITS_HPP

#include <memory>

namespace mstd {

template <class _Alloc>
struct __check_valid_allocator : std::true_type {
  using _Traits = std::allocator_traits<_Alloc>;
  using RebindAlloc = _Traits:: template rebind_alloc <typename _Traits::value_type>;
  static_assert(std::is_same_v<_Alloc, RebindAlloc>,
                "[allocator.requirements] states that rebinding an allocator to the same type should result in the "
                "original allocator");
};

} // namespace mstd


#endif // MSTD_MEMORY_ALLOCATOR_TRAITS_HPP