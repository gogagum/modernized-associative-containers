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

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

MSTD_PUSH_MACROS
#include <detail/undef_macros.hpp>

MSTD_BEGIN_NAMESPACE_STD

MSTD_SUPPRESS_DEPRECATED_PUSH
// __pointer
template <class _Tp>
using __pointer_member MSTD_NODEBUG = typename _Tp::pointer;

template <class _Tp, class _Alloc>
using __pointer MSTD_NODEBUG = __detected_or_t<_Tp*, __pointer_member, std::remove_reference_t<_Alloc> >;

// This trait returns _Alias<_Alloc> if that's well-formed, and _Ptr rebound to _Tp otherwise
template <class _Alloc, template <class> class _Alias, class _Ptr, class _Tp, class = void>
struct __rebind_or_alias_pointer {
#ifdef MSTD_CXX03_LANG
  using type MSTD_NODEBUG = typename std::pointer_traits<_Ptr>::template rebind<_Tp>::other;
#else
  using type MSTD_NODEBUG = typename std::pointer_traits<_Ptr>::template rebind<_Tp>;
#endif
};

template <class _Ptr, class _Alloc, class _Tp, template <class> class _Alias>
struct __rebind_or_alias_pointer<_Alloc, _Alias, _Ptr, _Tp, std::void_t<_Alias<_Alloc> > > {
  using type MSTD_NODEBUG = _Alias<_Alloc>;
};

// __const_pointer
template <class _Alloc>
using __const_pointer_member MSTD_NODEBUG = typename _Alloc::const_pointer;

template <class _Tp, class _Ptr, class _Alloc>
using __const_pointer_t MSTD_NODEBUG =
    typename __rebind_or_alias_pointer<_Alloc, __const_pointer_member, _Ptr, const _Tp>::type;
MSTD_SUPPRESS_DEPRECATED_POP

// __void_pointer
template <class _Alloc>
using __void_pointer_member MSTD_NODEBUG = typename _Alloc::void_pointer;

template <class _Ptr, class _Alloc>
using __void_pointer_t MSTD_NODEBUG =
    typename __rebind_or_alias_pointer<_Alloc, __void_pointer_member, _Ptr, void>::type;

// __const_void_pointer
template <class _Alloc>
using __const_void_pointer_member MSTD_NODEBUG = typename _Alloc::const_void_pointer;

template <class _Ptr, class _Alloc>
using __const_void_pointer_t MSTD_NODEBUG =
    typename __rebind_or_alias_pointer<_Alloc, __const_void_pointer_member, _Ptr, const void>::type;

// __size_type
template <class _Tp>
using __size_type_member MSTD_NODEBUG = typename _Tp::size_type;

template <class _Alloc, class _DiffType>
using __size_type MSTD_NODEBUG = __detected_or_t<std::make_unsigned_t<_DiffType>, __size_type_member, _Alloc>;

// __alloc_traits_difference_type
template <class _Alloc, class _Ptr, class = void>
struct __alloc_traits_difference_type {
  using type MSTD_NODEBUG = typename std::pointer_traits<_Ptr>::difference_type;
};

template <class _Alloc, class _Ptr>
struct __alloc_traits_difference_type<_Alloc, _Ptr, std::void_t<typename _Alloc::difference_type> > {
  using type MSTD_NODEBUG = typename _Alloc::difference_type;
};

// __propagate_on_container_copy_assignment
template <class _Tp>
using __propagate_on_container_copy_assignment_member MSTD_NODEBUG =
    typename _Tp::propagate_on_container_copy_assignment;

template <class _Alloc>
using __propagate_on_container_copy_assignment MSTD_NODEBUG =
    __detected_or_t<std::false_type, __propagate_on_container_copy_assignment_member, _Alloc>;

// __propagate_on_container_move_assignment
template <class _Tp>
using __propagate_on_container_move_assignment_member MSTD_NODEBUG =
    typename _Tp::propagate_on_container_move_assignment;

template <class _Alloc>
using __propagate_on_container_move_assignment MSTD_NODEBUG =
    __detected_or_t<std::false_type, __propagate_on_container_move_assignment_member, _Alloc>;

// __propagate_on_container_swap
template <class _Tp>
using __propagate_on_container_swap_member MSTD_NODEBUG = typename _Tp::propagate_on_container_swap;

template <class _Alloc>
using __propagate_on_container_swap MSTD_NODEBUG =
    __detected_or_t<std::false_type, __propagate_on_container_swap_member, _Alloc>;

MSTD_SUPPRESS_DEPRECATED_PUSH
// __is_always_equal
template <class _Tp>
using __is_always_equal_member MSTD_NODEBUG = typename _Tp::is_always_equal;

template <class _Alloc>
using __is_always_equal MSTD_NODEBUG =
    __detected_or_t<typename std::is_empty<_Alloc>::type, __is_always_equal_member, _Alloc>;

// __allocator_traits_rebind
template <class _Tp, class _Up, class = void>
inline const bool __has_rebind_other_v = false;
template <class _Tp, class _Up>
inline const bool __has_rebind_other_v<_Tp, _Up, std::void_t<typename _Tp::template rebind<_Up>::other> > = true;

template <class _Tp, class _Up, bool = __has_rebind_other_v<_Tp, _Up> >
struct __allocator_traits_rebind {
  static_assert(__has_rebind_other_v<_Tp, _Up>, "This allocator has to implement rebind");
  using type MSTD_NODEBUG = typename _Tp::template rebind<_Up>::other;
};
template <template <class, class...> class _Alloc, class _Tp, class... _Args, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _Args...>, _Up, true> {
  using type MSTD_NODEBUG = typename _Alloc<_Tp, _Args...>::template rebind<_Up>::other;
};
template <template <class, class...> class _Alloc, class _Tp, class... _Args, class _Up>
struct __allocator_traits_rebind<_Alloc<_Tp, _Args...>, _Up, false> {
  using type MSTD_NODEBUG = _Alloc<_Up, _Args...>;
};
MSTD_SUPPRESS_DEPRECATED_POP

template <class _Alloc, class _Tp>
using __allocator_traits_rebind_t MSTD_NODEBUG = typename __allocator_traits_rebind<_Alloc, _Tp>::type;

MSTD_SUPPRESS_DEPRECATED_PUSH

// __has_allocate_hint_v
template <class _Alloc, class _SizeType, class _ConstVoidPtr, class = void>
inline const bool __has_allocate_hint_v = false;

template <class _Alloc, class _SizeType, class _ConstVoidPtr>
inline const bool __has_allocate_hint_v<
    _Alloc,
    _SizeType,
    _ConstVoidPtr,
    decltype((void)std::declval<_Alloc>().allocate(std::declval<_SizeType>(), std::declval<_ConstVoidPtr>()))> = true;

// __has_construct_v
template <class, class _Alloc, class... _Args>
inline const bool __has_construct_impl = false;

template <class _Alloc, class... _Args>
inline const bool
    __has_construct_impl<decltype((void)std::declval<_Alloc>().construct(std::declval<_Args>()...)), _Alloc, _Args...> =
        true;

template <class _Alloc, class... _Args>
inline const bool __has_construct_v = __has_construct_impl<void, _Alloc, _Args...>;

// __has_destroy_v
template <class _Alloc, class _Pointer, class = void>
inline const bool __has_destroy_v = false;

template <class _Alloc, class _Pointer>
inline const bool
    __has_destroy_v<_Alloc, _Pointer, decltype((void)std::declval<_Alloc>().destroy(std::declval<_Pointer>()))> = true;

// __has_max_size_v
template <class _Alloc, class = void>
inline const bool __has_max_size_v = false;

template <class _Alloc>
inline const bool __has_max_size_v<_Alloc, decltype((void)std::declval<_Alloc&>().max_size())> = true;

// __has_select_on_container_copy_construction_v
template <class _Alloc, class = void>
inline const bool __has_select_on_container_copy_construction_v = false;

template <class _Alloc>
inline const bool __has_select_on_container_copy_construction_v<
    _Alloc,
    decltype((void)std::declval<_Alloc>().select_on_container_copy_construction())> = true;

MSTD_SUPPRESS_DEPRECATED_POP

#if MSTD_STD_VER >= 23

template <class _Pointer, class _SizeType = size_t>
struct allocation_result {
  _Pointer ptr;
  _SizeType count;
};
MSTD_CTAD_SUPPORTED_FOR_TYPE(allocation_result);

#endif // MSTD_STD_VER

template <class _Alloc>
struct allocator_traits {
  using allocator_type                         = _Alloc;
  using value_type                             = typename allocator_type::value_type;
  using pointer                                = __pointer<value_type, allocator_type>;
  using const_pointer                          = __const_pointer_t<value_type, pointer, allocator_type>;
  using void_pointer                           = __void_pointer_t<pointer, allocator_type>;
  using const_void_pointer                     = __const_void_pointer_t<pointer, allocator_type>;
  using difference_type                        = typename __alloc_traits_difference_type<allocator_type, pointer>::type;
  using size_type                              = __size_type<allocator_type, difference_type>;
  using propagate_on_container_copy_assignment = __propagate_on_container_copy_assignment<allocator_type>;
  using propagate_on_container_move_assignment = __propagate_on_container_move_assignment<allocator_type>;
  using propagate_on_container_swap            = __propagate_on_container_swap<allocator_type>;
  using is_always_equal                        = __is_always_equal<allocator_type>;

#ifndef MSTD_CXX03_LANG
  template <class _Tp>
  using rebind_alloc = __allocator_traits_rebind_t<allocator_type, _Tp>;
  template <class _Tp>
  using rebind_traits = allocator_traits<rebind_alloc<_Tp> >;
#else  // MSTD_CXX03_LANG
  template <class _Tp>
  struct rebind_alloc {
    using other = __allocator_traits_rebind_t<allocator_type, _Tp>;
  };
  template <class _Tp>
  struct rebind_traits {
    using other = allocator_traits<typename rebind_alloc<_Tp>::other>;
  };
#endif // MSTD_CXX03_LANG

  [[__nodiscard__]] MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static pointer
  allocate(allocator_type& __a, size_type __n) {
    return __a.allocate(__n);
  }

  template <class _Ap = _Alloc, std::enable_if_t<__has_allocate_hint_v<_Ap, size_type, const_void_pointer>, int> = 0>
  [[__nodiscard__]] MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static pointer
  allocate(allocator_type& __a, size_type __n, const_void_pointer __hint) {
    MSTD_SUPPRESS_DEPRECATED_PUSH
    return __a.allocate(__n, __hint);
    MSTD_SUPPRESS_DEPRECATED_POP
  }
  template <class _Ap = _Alloc, std::enable_if_t<!__has_allocate_hint_v<_Ap, size_type, const_void_pointer>, int> = 0>
  [[__nodiscard__]] MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static pointer
  allocate(allocator_type& __a, size_type __n, const_void_pointer) {
    return __a.allocate(__n);
  }

#if MSTD_STD_VER >= 23
  template <class _Ap = _Alloc>
  [[nodiscard]] MSTD_HIDE_FROM_ABI static constexpr allocation_result<pointer, size_type>
  allocate_at_least(_Ap& __alloc, size_type __n) {
    if constexpr (requires { __alloc.allocate_at_least(__n); }) {
      return __alloc.allocate_at_least(__n);
    } else {
      return {__alloc.allocate(__n), __n};
    }
  }
#endif

  MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static void
  deallocate(allocator_type& __a, pointer __p, size_type __n) _NOEXCEPT {
    __a.deallocate(__p, __n);
  }

  template <class _Tp, class... _Args, std::enable_if_t<__has_construct_v<allocator_type, _Tp*, _Args...>, int> = 0>
  MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static void
  construct(allocator_type& __a, _Tp* __p, _Args&&... __args) {
    MSTD_SUPPRESS_DEPRECATED_PUSH
    __a.construct(__p, std::forward<_Args>(__args)...);
    MSTD_SUPPRESS_DEPRECATED_POP
  }
  template <class _Tp, class... _Args, std::enable_if_t<!__has_construct_v<allocator_type, _Tp*, _Args...>, int> = 0>
  MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static void
  construct(allocator_type&, _Tp* __p, _Args&&... __args) {
    std::construct_at(__p, std::forward<_Args>(__args)...);
  }

  template <class _Tp, std::enable_if_t<__has_destroy_v<allocator_type, _Tp*>, int> = 0>
  MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static void destroy(allocator_type& __a, _Tp* __p) {
    MSTD_SUPPRESS_DEPRECATED_PUSH
    __a.destroy(__p);
    MSTD_SUPPRESS_DEPRECATED_POP
  }
  template <class _Tp, std::enable_if_t<!__has_destroy_v<allocator_type, _Tp*>, int> = 0>
  MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static void destroy(allocator_type&, _Tp* __p) {
    std::destroy_at(__p);
  }

  template <class _Ap = _Alloc, std::enable_if_t<__has_max_size_v<const _Ap>, int> = 0>
  [[__nodiscard__]] MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static size_type
  max_size(const allocator_type& __a) _NOEXCEPT {
    MSTD_SUPPRESS_DEPRECATED_PUSH
    return __a.max_size();
    MSTD_SUPPRESS_DEPRECATED_POP
  }
  template <class _Ap = _Alloc, std::enable_if_t<!__has_max_size_v<const _Ap>, int> = 0>
  [[__nodiscard__]] MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static size_type
  max_size(const allocator_type&) _NOEXCEPT {
    return std::numeric_limits<size_type>::max() / sizeof(value_type);
  }

  template <class _Ap = _Alloc, std::enable_if_t<__has_select_on_container_copy_construction_v<const _Ap>, int> = 0>
  [[__nodiscard__]] MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static allocator_type
  select_on_container_copy_construction(const allocator_type& __a) {
    return __a.select_on_container_copy_construction();
  }
  template <class _Ap = _Alloc, std::enable_if_t<!__has_select_on_container_copy_construction_v<const _Ap>, int> = 0>
  [[__nodiscard__]] MSTD_HIDE_FROM_ABI MSTD_CONSTEXPR_SINCE_CXX20 static allocator_type
  select_on_container_copy_construction(const allocator_type& __a) {
    return __a;
  }
};

#ifndef MSTD_CXX03_LANG
template <class _Traits, class _Tp>
using __rebind_alloc MSTD_NODEBUG = typename _Traits::template rebind_alloc<_Tp>;
#else
template <class _Traits, class _Tp>
using __rebind_alloc MSTD_NODEBUG = typename _Traits::template rebind_alloc<_Tp>::other;
#endif

template <class _Alloc>
struct __check_valid_allocator : std::true_type {
  using _Traits MSTD_NODEBUG = std::allocator_traits<_Alloc>;
  static_assert(std::is_same<_Alloc, __rebind_alloc<_Traits, typename _Traits::value_type> >::value,
                "[allocator.requirements] states that rebinding an allocator to the same type should result in the "
                "original allocator");
};

// __is_default_allocator_v
template <class _Tp>
inline const bool __is_std_allocator_v = false;

template <class _Tp>
inline const bool __is_std_allocator_v<std::allocator<_Tp> > = true;

// __is_cpp17_move_insertable_v
template <class _Alloc>
inline const bool __is_cpp17_move_insertable_v =
    std::is_move_constructible<typename _Alloc::value_type>::value ||
    (!__is_std_allocator_v<_Alloc> &&
     __has_construct_v<_Alloc, typename _Alloc::value_type*, typename _Alloc::value_type&&>);

// __is_cpp17_copy_insertable_v
template <class _Alloc>
inline const bool __is_cpp17_copy_insertable_v =
    __is_cpp17_move_insertable_v<_Alloc> &&
    (std::is_copy_constructible<typename _Alloc::value_type>::value ||
     (!__is_std_allocator_v<_Alloc> &&
      __has_construct_v<_Alloc, typename _Alloc::value_type*, const typename _Alloc::value_type&>));

MSTD_END_NAMESPACE

MSTD_POP_MACROS

#endif // MSTD_MEMORY_ALLOCATOR_TRAITS_HPP