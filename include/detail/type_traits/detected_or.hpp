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

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

template <class _Default, class _Void, template <class...> class _Op, class... _Args>
struct __detector {
  using type MSTD_NODEBUG = _Default;
};

template <class _Default, template <class...> class _Op, class... _Args>
struct __detector<_Default, std::void_t<_Op<_Args...> >, _Op, _Args...> {
  using type MSTD_NODEBUG = _Op<_Args...>;
};

template <class _Default, template <class...> class _Op, class... _Args>
using __detected_or_t MSTD_NODEBUG = typename __detector<_Default, void, _Op, _Args...>::type;

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_DETECTED_OR_HPP
