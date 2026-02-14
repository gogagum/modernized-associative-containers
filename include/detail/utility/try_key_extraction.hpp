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
#include <tuple>

namespace mstd {

template <class T, class KeyT>
concept reference_to = std::same_as<KeyT, std::remove_const_t<std::remove_reference_t<T>>>;

template <class KeyT, class WithKeyT, reference_to<KeyT> ArgT>
decltype(auto) try_key_extraction_with_key(WithKeyT with_key, ArgT&& arg) {
  return with_key(arg, std::forward<ArgT>(arg));
}

template <class KeyT, class WithKeyT, reference_to<KeyT> T1, class T2>
decltype(auto) try_key_extraction_with_key(WithKeyT with_key, std::pair<T1, T2>&& arg) {
  return with_key(arg.first, std::move(arg));
}

template <class KeyT, class WithKeyT, reference_to<KeyT> T1, class T2>
decltype(auto) try_key_extraction_with_key(WithKeyT with_key, const std::pair<T1, T2>& arg) {
  return with_key(arg.first, arg);
}

template <class KeyT, class WithKeyT, reference_to<KeyT> ArgT1, class ArgT2>
decltype(auto) try_key_extraction_with_key(WithKeyT with_key, ArgT1&& arg1, ArgT2&& arg2) {
  return with_key(arg1, std::forward<ArgT1>(arg1), std::forward<ArgT2>(arg2));
}

template <class KeyT, class WithKeyT, reference_to<std::piecewise_construct_t> PiecewiseConstructT, reference_to<KeyT> T1, class TupleT2>
decltype(auto) try_key_extraction_with_key(WithKeyT with_key, PiecewiseConstructT&& pc, std::tuple<T1>&& tuple1, TupleT2&& tuple2) {
  return with_key(std::get<0>(tuple1), std::forward<PiecewiseConstructT>(pc), std::move(tuple1), std::forward<TupleT2>(tuple2));
}

template <class KeyT, class WithKeyT, reference_to<std::piecewise_construct_t> PiecewiseConstructT, reference_to<KeyT> T1, class TupleT2>
decltype(auto) try_key_extraction_with_key(WithKeyT with_key, PiecewiseConstructT&& pc, const std::tuple<T1>& tuple1, TupleT2&& tuple2) {
  return with_key(std::get<0>(tuple1), std::forward<PiecewiseConstructT>(pc), tuple1, std::forward<TupleT2>(tuple2));
}

// This function tries extracting the given KeyT from ArgsT...
// If it succeeds to extract the key, it calls the `with_key` function with the extracted key and all of the
// arguments. Otherwise it calls the `without_key` function with all of the arguments.
//
// Both `with_key` and `without_key` must take all arguments by reference.
template <class KeyT, class WithKeyT, class WithoutKeyT, class... ArgsT>
decltype(auto) try_key_extraction(WithKeyT with_key, WithoutKeyT without_key, ArgsT&&... args) {
  if constexpr (requires { try_key_extraction_with_key<KeyT>(with_key, std::forward<ArgsT>(args)...); }) {
    return try_key_extraction_with_key<KeyT>(with_key, std::forward<ArgsT>(args)...);
  } else {
    return without_key(std::forward<ArgsT>(args)...);
  }
}

} // namespace mstd

#endif // MSTD_UTILITY_TRY_EXTRACT_KEY_HPP
