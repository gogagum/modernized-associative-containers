//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_DATASIZEOF_HPP
#define MSTD_TYPE_TRAITS_DATASIZEOF_HPP

#include <detail/config.hpp>
#include <cstddef>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

// This trait provides the size of a type excluding any tail padding.
//
// It is useful in contexts where performing an operation using the full size of the class (including padding) may
// have unintended side effects, such as overwriting a derived class' member when writing the tail padding of a class
// through a pointer-to-base.

namespace mstd {

template <class _Tp>
struct _FirstPaddingByte {
  [[no_unique_address]] _Tp __v_;
  char __first_padding_byte_;
};

// _FirstPaddingByte<> is sometimes non-standard layout.
// It is conditionally-supported to use __builtin_offsetof in that case, but GCC and Clang allow it.
template <class _Tp>
inline const size_t __datasizeof_v = __builtin_offsetof(_FirstPaddingByte<_Tp>, __first_padding_byte_);

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_DATASIZEOF_HPP
