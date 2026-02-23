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

#include <cstddef>
#include <memory>
#include <utility>
#include <limits>

namespace mstd {

// __allocator_traits_rebind
template <class _Tp, class _Up, class = void>
inline const bool __has_rebind_other_v = false;
template <class _Tp, class _Up>
inline const bool __has_rebind_other_v<_Tp, _Up, std::void_t<typename _Tp::template rebind<_Up>::other> > = true;

template <class _Tp, class _Up, bool = __has_rebind_other_v<_Tp, _Up> >
struct __allocator_traits_rebind {
  static_assert(__has_rebind_other_v<_Tp, _Up>, "This allocator has to implement rebind");
  using type = typename _Tp::template rebind<_Up>::other;
};
template <template <class, class...> class _Alloc, class _Tp, class... ArgsT, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, ArgsT...>, _Up, true> {
  using type = typename _Alloc<_Tp, ArgsT...>::template rebind<_Up>::other;
};
template <template <class, class...> class _Alloc, class _Tp, class... ArgsT, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, ArgsT...>, _Up, false> {
  using type = _Alloc<_Up, ArgsT...>;
};

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