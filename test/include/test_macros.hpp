// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef SUPPORT_TEST_MACROS_HPP
#define SUPPORT_TEST_MACROS_HPP

#ifdef __has_include
#  if __has_include(<version>)
#    include <version>
#  else
#    include <ciso646>
#  endif
#else
#  include <ciso646>
#endif

#define TEST_STRINGIZE_IMPL(...) #__VA_ARGS__
#define TEST_STRINGIZE(...) TEST_STRINGIZE_IMPL(__VA_ARGS__)

#ifndef __has_include
#define __has_include(...) 0
#endif

// Attempt to deduce the GLIBC version
#if (defined(__has_include) && __has_include(<features.h>)) || defined(__linux__)
#include <features.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
// This function can be used to hide some objects from compiler optimizations.
//
// For example, this is useful to hide the result of a call to `new` and ensure
// that the compiler doesn't elide the call to new/delete. Otherwise, elliding
// calls to new/delete is allowed by the Standard and compilers actually do it
// when optimizations are enabled.
template <class Tp>
inline Tp const& DoNotOptimize(Tp const& value) {
  // The `m` constraint is invalid in the AMDGPU backend.
#  if defined(__AMDGPU__) || defined(__NVPTX__)
  asm volatile("" : : "r"(value) : "memory");
#  else
  asm volatile("" : : "r,m"(value) : "memory");
#  endif
  return value;
}

template <class Tp>
inline Tp& DoNotOptimize(Tp& value) {
  // The `m` and `r` output constraint is invalid in the AMDGPU backend as well
  // as i8 / i1 arguments, so we just capture the pointer instead.
#  if defined(__AMDGPU__)
  Tp* tmp = &value;
  asm volatile("" : "+v"(tmp) : : "memory");
#  elif defined(__clang__)
  asm volatile("" : "+r,m"(value) : : "memory");
#  else
  asm volatile("" : "+m,r"(value) : : "memory");
#  endif
  return value;
}
#else
#include <intrin.h>
template <class Tp>
inline Tp const& DoNotOptimize(Tp const& value) {
  const volatile void* volatile unused = __builtin_addressof(value);
  static_cast<void>(unused);
  _ReadWriteBarrier();
  return value;
}
#endif

#endif // SUPPORT_TEST_MACROS_HPP
