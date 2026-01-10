//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_MEMORY_SWAP_ALLOCATOR_HPP
#define MSTD_MEMORY_SWAP_ALLOCATOR_HPP

#include <detail/config.hpp>
#include "allocator_traits.hpp"
#include <utility>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

template <typename _Alloc>
MSTD_HIDE_FROM_ABI constexpr void __swap_allocator(_Alloc& __a1, _Alloc& __a2, std::true_type) noexcept
{
  std::swap(__a1, __a2);
}

template <typename _Alloc>
inline MSTD_HIDE_FROM_ABI constexpr void
__swap_allocator(_Alloc&, _Alloc&, std::false_type) noexcept {}

template <typename _Alloc>
inline MSTD_HIDE_FROM_ABI constexpr void __swap_allocator(_Alloc& __a1, _Alloc& __a2) noexcept
{
  mstd::__swap_allocator(
      __a1, __a2, std::integral_constant<bool, std::allocator_traits<_Alloc>::propagate_on_container_swap::value>());
}

} // namespace mstd

#endif // MSTD_MEMORY_SWAP_ALLOCATOR_HPP
