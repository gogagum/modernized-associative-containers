//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_COPY_CVREF_HPP
#define MSTD_TYPE_TRAITS_COPY_CVREF_HPP

#include <detail/config.hpp>
#include "copy_cv.hpp"

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

template <class _From>
struct __copy_cvref {
  template <class _To>
  using __apply MSTD_NODEBUG = __copy_cv_t<_From, _To>;
};

template <class _From>
struct __copy_cvref<_From&> {
  template <class _To>
  using __apply MSTD_NODEBUG = std::add_lvalue_reference_t<__copy_cv_t<_From, _To> >;
};

template <class _From>
struct __copy_cvref<_From&&> {
  template <class _To>
  using __apply MSTD_NODEBUG = std::add_rvalue_reference_t<__copy_cv_t<_From, _To> >;
};

template <class _From, class _To>
using __copy_cvref_t MSTD_NODEBUG = typename __copy_cvref<_From>::template __apply<_To>;

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_COPY_CVREF_HPP
