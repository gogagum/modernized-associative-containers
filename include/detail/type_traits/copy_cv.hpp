//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_COPY_CV_HPP
#define MSTD_TYPE_TRAITS_COPY_CV_HPP

namespace mstd {

// Let COPYCV(FROM, TO) be an alias for type TO with the addition of FROM's
// top-level cv-qualifiers.
template <class FromT>
struct __copy_cv {
  template <class ToT>
  using __apply = ToT;
};

template <class FromT>
struct __copy_cv<const FromT> {
  template <class ToT>
  using __apply = const ToT;
};

template <class FromT>
struct __copy_cv<volatile FromT> {
  template <class ToT>
  using __apply = volatile ToT;
};

template <class FromT>
struct __copy_cv<const volatile FromT> {
  template <class ToT>
  using __apply = const volatile ToT;
};

template <class FromT, class ToT>
using __copy_cv_t = typename __copy_cv<FromT>::template __apply<ToT>;

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_COPY_CV_HPP
