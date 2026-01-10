//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_UTILITY_TRY_EXTRACT_KEY_HPP
#define MSTD_UTILITY_TRY_EXTRACT_KEY_HPP

#include <detail/config.hpp>
#include <utility>
#include <type_traits>
#include <detail/type_traits/remove_const_ref.hpp>
#include <detail/utility/priority_tag.hpp>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

MSTD_BEGIN_NAMESPACE_STD

template <class _Type>
inline const bool __is_pair_v = false;

template <class _Type1, class _Type2>
inline const bool __is_pair_v<std::pair<_Type1, _Type2> > = true;

template <class>
inline const bool __is_tuple_v = false;

template <class... _Tp>
inline const bool __is_tuple_v<std::tuple<_Tp...>> = true;

template <class _KeyT, class _Ret, class _WithKey, class _WithoutKey, class... _Args>
MSTD_HIDE_FROM_ABI _Ret
__try_key_extraction_impl(__priority_tag<0>, _WithKey, _WithoutKey __without_key, _Args&&... __args) {
  return __without_key(std::forward<_Args>(__args)...);
}

template <class _KeyT,
          class _Ret,
          class _WithKey,
          class _WithoutKey,
          class _Arg,
          std::enable_if_t<std::is_same<_KeyT, __remove_const_ref_t<_Arg> >::value, int> = 0>
MSTD_HIDE_FROM_ABI _Ret
__try_key_extraction_impl(__priority_tag<1>, _WithKey __with_key, _WithoutKey, _Arg&& __arg) {
  return __with_key(__arg, std::forward<_Arg>(__arg));
}

template <class _KeyT,
          class _Ret,
          class _WithKey,
          class _WithoutKey,
          class _Arg,
          std::enable_if_t<__is_pair_v<__remove_const_ref_t<_Arg> > &&
                            std::is_same<std::remove_const_t<typename __remove_const_ref_t<_Arg>::first_type>, _KeyT>::value,
                        int> = 0>
MSTD_HIDE_FROM_ABI _Ret
__try_key_extraction_impl(__priority_tag<1>, _WithKey __with_key, _WithoutKey, _Arg&& __arg) {
  return __with_key(__arg.first, std::forward<_Arg>(__arg));
}

template <class _KeyT,
          class _Ret,
          class _WithKey,
          class _WithoutKey,
          class _Arg1,
          class _Arg2,
          std::enable_if_t<std::is_same<_KeyT, __remove_const_ref_t<_Arg1> >::value, int> = 0>
MSTD_HIDE_FROM_ABI _Ret
__try_key_extraction_impl(__priority_tag<1>, _WithKey __with_key, _WithoutKey, _Arg1&& __arg1, _Arg2&& __arg2) {
  return __with_key(__arg1, std::forward<_Arg1>(__arg1), std::forward<_Arg2>(__arg2));
}

template <class _KeyT,
          class _Ret,
          class _WithKey,
          class _WithoutKey,
          class _PiecewiseConstruct,
          class _Tuple1,
          class _Tuple2,
          std::enable_if_t<std::is_same<__remove_const_ref_t<_PiecewiseConstruct>, std::piecewise_construct_t>::value &&
                            __is_tuple_v<_Tuple1> && std::tuple_size<_Tuple1>::value == 1 &&
                            std::is_same<__remove_const_ref_t<typename std::tuple_element<0, _Tuple1>::type>, _KeyT>::value,
                        int> = 0>
MSTD_HIDE_FROM_ABI _Ret __try_key_extraction_impl(
    __priority_tag<1>,
    _WithKey __with_key,
    _WithoutKey,
    _PiecewiseConstruct&& __pc,
    _Tuple1&& __tuple1,
    _Tuple2&& __tuple2) {
  return __with_key(
      std::get<0>(__tuple1),
      std::forward<_PiecewiseConstruct>(__pc),
      std::forward<_Tuple1>(__tuple1),
      std::forward<_Tuple2>(__tuple2));
}

// This function tries extracting the given _KeyT from _Args...
// If it succeeds to extract the key, it calls the `__with_key` function with the extracted key and all of the
// arguments. Otherwise it calls the `__without_key` function with all of the arguments.
//
// Both `__with_key` and `__without_key` must take all arguments by reference.
template <class _KeyT, class _WithKey, class _WithoutKey, class... _Args>
MSTD_HIDE_FROM_ABI decltype(std::declval<_WithoutKey>()(std::declval<_Args>()...))
__try_key_extraction(_WithKey __with_key, _WithoutKey __without_key, _Args&&... __args) {
  using _Ret = decltype(__without_key(std::forward<_Args>(__args)...));
  return mstd::__try_key_extraction_impl<_KeyT, _Ret>(
      __priority_tag<1>(), __with_key, __without_key, std::forward<_Args>(__args)...);
}

MSTD_END_NAMESPACE

#endif // MSTD_UTILITY_TRY_EXTRACT_KEY_HPP
