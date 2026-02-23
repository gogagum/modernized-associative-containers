//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_COPY_CVREF_HPP
#define MSTD_TYPE_TRAITS_COPY_CVREF_HPP

#include "copy_cv.hpp"

namespace mstd {

template <class FromT>
struct __copy_cvref {
  template <class ToT>
  using __apply = __copy_cv_t<FromT, ToT>;
};

template <class FromT>
struct __copy_cvref<FromT&> {
  template <class ToT>
  using __apply = std::add_lvalue_reference_t<__copy_cv_t<FromT, ToT> >;
};

template <class FromT>
struct __copy_cvref<FromT&&> {
  template <class ToT>
  using __apply = std::add_rvalue_reference_t<__copy_cv_t<FromT, ToT> >;
};

template <class FromT, class ToT>
using __copy_cvref_t = typename __copy_cvref<FromT>::template __apply<ToT>;

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_COPY_CVREF_HPP
