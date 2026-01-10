// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_CONFIG_HPP
#define MSTD_CONFIG_HPP

#include "configuration/compiler.hpp"

#ifdef __cplusplus

#  define MSTD_CONCAT_IMPL(_X, _Y) _X##_Y
#  define MSTD_CONCAT(_X, _Y) MSTD_CONCAT_IMPL(_X, _Y)
#  define MSTD_CONCAT3(X, Y, Z) MSTD_CONCAT(X, MSTD_CONCAT(Y, Z))

// There are a handful of public standard library types that are intended to
// support CTAD but don't need any explicit deduction guides to do so. This
// macro is used to mark them as such, which suppresses the
// '-Wctad-maybe-unsupported' compiler warning when CTAD is used in user code
// with these classes.
#  ifdef MSTD_COMPILER_CLANG_BASED
#    define MSTD_CTAD_SUPPORTED_FOR_TYPE(_ClassName)                                     \
      template <class... _Tag>                                                           \
      [[maybe_unused]] _ClassName(typename _Tag::__allow_ctad...)->_ClassName<_Tag...>
#  else
#    define MSTD_CTAD_SUPPORTED_FOR_TYPE(ClassName)                                      \
      template <class... _Tag>                                                           \
      ClassName(typename _Tag::__allow_ctad...)->ClassName<_Tag...>
#  endif

#endif // __cplusplus

#endif // MSTD___CONFIG
