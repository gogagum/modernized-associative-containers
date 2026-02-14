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

#include <detail/config.hpp>
#include <cstddef>
#include <memory>
#include <detail/type_traits/detected_or.hpp>
#include <utility>
#include <limits>

namespace mstd {

// __pointer
template <class _Tp>
using __pointer_member = typename _Tp::pointer;

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
template <template <class, class...> class _Alloc, class _Tp, class... _Args, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _Args...>, _Up, true> {
  using type = typename _Alloc<_Tp, _Args...>::template rebind<_Up>::other;
};
template <template <class, class...> class _Alloc, class _Tp, class... _Args, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _Args...>, _Up, false> {
  using type = _Alloc<_Up, _Args...>;
};

template <class _Traits, class _Tp>
using __rebind_alloc = typename _Traits::template rebind_alloc<_Tp>;

template <class _Alloc>
struct __check_valid_allocator : std::true_type {
  using _Traits = std::allocator_traits<_Alloc>;
  static_assert(std::is_same<_Alloc, __rebind_alloc<_Traits, typename _Traits::value_type> >::value,
                "[allocator.requirements] states that rebinding an allocator to the same type should result in the "
                "original allocator");
};

} // namespace mstd


#endif // MSTD_MEMORY_ALLOCATOR_TRAITS_HPP