//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_DETECTED_OR_HPP
#define MSTD_TYPE_TRAITS_DETECTED_OR_HPP

#include <detail/config.hpp>

namespace mstd {

template <class _Default, class _Void, template <class...> class _Op, class... ArgsT>
struct __detector {
  using type = _Default;
};

template <class _Default, template <class...> class _Op, class... ArgsT>
struct __detector<_Default, std::void_t<_Op<ArgsT...> >, _Op, ArgsT...> {
  using type = _Op<ArgsT...>;
};

template <class _Default, template <class...> class _Op, class... ArgsT>
using __detected_or_t = typename __detector<_Default, void, _Op, ArgsT...>::type;

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_DETECTED_OR_HPP
