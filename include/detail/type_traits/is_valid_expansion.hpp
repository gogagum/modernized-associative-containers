//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_IS_VALID_EXPANSION_HPP
#define MSTD_TYPE_TRAITS_IS_VALID_EXPANSION_HPP

#include <detail/config.hpp>
#include <type_traits>

namespace mstd {

template <template <class...> class _Templ, class... _Args, class = _Templ<_Args...> >
std::true_type __sfinae_test_impl(int);
template <template <class...> class, class...>
std::false_type __sfinae_test_impl(...);

template <template <class...> class _Templ, class... _Args>
using _IsValidExpansion = decltype(mstd::__sfinae_test_impl<_Templ, _Args...>(0));

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_IS_VALID_EXPANSION_HPP
