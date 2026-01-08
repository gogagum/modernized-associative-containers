// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_CONFIGURATION_COMPILER_HPP
#define MSTD_CONFIGURATION_COMPILER_HPP

#ifndef MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER
#  pragma GCC system_header
#endif

#if defined(__apple_build_version__)
// Given AppleClang XX.Y.Z, MSTD_APPLE_CLANG_VER is XXYZ (e.g. AppleClang 14.0.3 => 1403)
#  define MSTD_COMPILER_CLANG_BASED
#  define MSTD_APPLE_CLANG_VER (__apple_build_version__ / 10000)
#elif defined(__clang__)
#  define MSTD_COMPILER_CLANG_BASED
#  define MSTD_CLANG_VER (__clang_major__ * 100 + __clang_minor__)
#elif defined(__GNUC__)
#  define MSTD_COMPILER_GCC
#  define MSTD_GCC_VER (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

#ifdef __cplusplus

// Warn if a compiler version is used that is not supported anymore
// LLVM RELEASE Update the minimum compiler versions
#  if defined(MSTD_CLANG_VER)
#    if MSTD_CLANG_VER < 2001
#      warning "Libc++ only supports Clang 20 and later"
#    endif
#  elif defined(MSTD_APPLE_CLANG_VER)
#    if MSTD_APPLE_CLANG_VER < 1700
#      warning "Libc++ only supports AppleClang 26 and later"
#    endif
#  elif defined(MSTD_GCC_VER)
#    if MSTD_GCC_VER < 1500
#      warning "Libc++ only supports GCC 15 and later"
#    endif
#  endif

#endif

#endif // MSTD_CONFIGURATION_COMPILER_HPP
