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

namespace mstd {

template <class _Type>
inline const bool __is_pair_v = false;

template <class _Type1, class _Type2>
inline const bool __is_pair_v<std::pair<_Type1, _Type2> > = true;

template <class>
inline const bool __is_tuple_v = false;

template <class... _Tp>
inline const bool __is_tuple_v<std::tuple<_Tp...>> = true;

template <class KeyT, class WithKeyT, class WithoutKeyT, class... ArgsT>
decltype(auto) __try_key_extraction_impl(__priority_tag<0>, WithKeyT, WithoutKeyT without_key, ArgsT&&... args) {
  return without_key(std::forward<ArgsT>(args)...);
}

template <class KeyT, class WithKeyT, class WithoutKeyT, class ArgT>
requires std::same_as<KeyT, __remove_const_ref_t<ArgT>>
decltype(auto) __try_key_extraction_impl(__priority_tag<1>, WithKeyT with_key, WithoutKeyT, ArgT&& arg) {
  return with_key(arg, std::forward<ArgT>(arg));
}

template <class KeyT, class WithKeyT, class WithoutKeyT, class ArgT>
requires __is_pair_v<__remove_const_ref_t<ArgT> >
         && std::same_as<std::remove_const_t<typename __remove_const_ref_t<ArgT>::first_type>, KeyT>
decltype(auto) __try_key_extraction_impl(__priority_tag<1>, WithKeyT with_key, WithoutKeyT, ArgT&& arg) {
  return with_key(arg.first, std::forward<ArgT>(arg));
}

template <class KeyT, class WithKeyT, class WithoutKeyT, class ArgT1, class ArgT2>
requires std::same_as<KeyT, __remove_const_ref_t<ArgT1> >
decltype(auto) __try_key_extraction_impl(__priority_tag<1>, WithKeyT with_key, WithoutKeyT, ArgT1&& arg1, ArgT2&& arg2) {
  return with_key(arg1, std::forward<ArgT1>(arg1), std::forward<ArgT2>(arg2));
}

template <class KeyT, class WithKeyT, class WithoutKeyT, class PiecewiseConstructT, class TupleT1, class TupleT2>
requires std::same_as<__remove_const_ref_t<PiecewiseConstructT>, std::piecewise_construct_t>
          && __is_tuple_v<TupleT1>
          && (std::tuple_size<TupleT1>::value == 1)
          && std::same_as<__remove_const_ref_t<typename std::tuple_element<0, TupleT1>::type>, KeyT>
decltype(auto) __try_key_extraction_impl(__priority_tag<1>, WithKeyT with_key,
                                         WithoutKeyT, PiecewiseConstructT&& pc,
                                         TupleT1&& tuple1, TupleT2&& tuple2) {
  return with_key(
      std::get<0>(tuple1),
      std::forward<PiecewiseConstructT>(pc),
      std::forward<TupleT1>(tuple1),
      std::forward<TupleT2>(tuple2));
}

// This function tries extracting the given KeyT from ArgsT...
// If it succeeds to extract the key, it calls the `with_key` function with the extracted key and all of the
// arguments. Otherwise it calls the `without_key` function with all of the arguments.
//
// Both `with_key` and `without_key` must take all arguments by reference.
template <class KeyT, class WithKeyT, class WithoutKeyT, class... ArgsT>
decltype(auto) __try_key_extraction(WithKeyT with_key, WithoutKeyT without_key, ArgsT&&... args) {
  return mstd::__try_key_extraction_impl<KeyT>(__priority_tag<1>(), with_key, without_key, std::forward<ArgsT>(args)...);
}

} // namespace mstd

#endif // MSTD_UTILITY_TRY_EXTRACT_KEY_HPP
