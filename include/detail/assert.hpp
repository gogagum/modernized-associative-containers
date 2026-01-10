// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_DETAIL_ASSERT_HPP
#define MSTD_DETAIL_ASSERT_HPP

#include "config.hpp"

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

#define MSTD_ASSERT(expression, message)                                                                            \
  (__builtin_expect(static_cast<bool>(expression), 1)                                                                  \
       ? (void)0                                                                                                       \
       : MSTD_ASSERTION_HANDLER(__FILE__ ":" MSTD_TOSTRING(                                                      \
             __LINE__) ": libc++ Hardening assertion " MSTD_TOSTRING(expression) " failed: " message "\n"))

// WARNING: __builtin_assume can currently inhibit optimizations. Only add assumptions with a clear
// optimization intent. See https://discourse.llvm.org/t/llvm-assume-blocks-optimization/71609 for a
// discussion.
#if __has_builtin(__builtin_assume)
#  define MSTD_ASSUME(expression) (__builtin_assume(static_cast<bool>(expression)))
#else
#  define MSTD_ASSUME(expression) ((void)0)
#endif

// clang-format off
// Fast hardening mode checks.

#if MSTD_HARDENING_MODE == MSTD_HARDENING_MODE_FAST

// Enabled checks.
#  define MSTD_ASSERT_VALID_INPUT_RANGE(expression, message)       MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_VALID_ELEMENT_ACCESS(expression, message)    MSTD_ASSERT(expression, message)
// Disabled checks.
// On most modern platforms, dereferencing a null pointer does not lead to an actual memory access.
#  define MSTD_ASSERT_NON_NULL(expression, message)                ((void)0)
// Overlapping ranges will make algorithms produce incorrect results but don't directly lead to a security
// vulnerability.
#  define MSTD_ASSERT_NON_OVERLAPPING_RANGES(expression, message)  ((void)0)
#  define MSTD_ASSERT_VALID_DEALLOCATION(expression, message)      ((void)0)
#  define MSTD_ASSERT_VALID_EXTERNAL_API_CALL(expression, message) ((void)0)
#  define MSTD_ASSERT_COMPATIBLE_ALLOCATOR(expression, message)    ((void)0)
#  define MSTD_ASSERT_ARGUMENT_WITHIN_DOMAIN(expression, message)  ((void)0)
#  define MSTD_ASSERT_PEDANTIC(expression, message)                ((void)0)
#  define MSTD_ASSERT_SEMANTIC_REQUIREMENT(expression, message)    ((void)0)
#  define MSTD_ASSERT_INTERNAL(expression, message)                ((void)0)
#  define MSTD_ASSERT_UNCATEGORIZED(expression, message)           ((void)0)

// Extensive hardening mode checks.

#elif MSTD_HARDENING_MODE == MSTD_HARDENING_MODE_EXTENSIVE

// Enabled checks.
#  define MSTD_ASSERT_VALID_INPUT_RANGE(expression, message)       MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_VALID_ELEMENT_ACCESS(expression, message)    MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_NON_NULL(expression, message)                MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_NON_OVERLAPPING_RANGES(expression, message)  MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_VALID_DEALLOCATION(expression, message)      MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_VALID_EXTERNAL_API_CALL(expression, message) MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_COMPATIBLE_ALLOCATOR(expression, message)    MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_ARGUMENT_WITHIN_DOMAIN(expression, message)  MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_PEDANTIC(expression, message)                MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_UNCATEGORIZED(expression, message)           MSTD_ASSERT(expression, message)
// Disabled checks.
#  define MSTD_ASSERT_SEMANTIC_REQUIREMENT(expression, message)    ((void)0)
#  define MSTD_ASSERT_INTERNAL(expression, message)                ((void)0)

// Debug hardening mode checks.

#elif MSTD_HARDENING_MODE == MSTD_HARDENING_MODE_DEBUG

// All checks enabled.
#  define MSTD_ASSERT_VALID_INPUT_RANGE(expression, message)       MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_VALID_ELEMENT_ACCESS(expression, message)    MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_NON_NULL(expression, message)                MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_NON_OVERLAPPING_RANGES(expression, message)  MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_VALID_DEALLOCATION(expression, message)      MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_VALID_EXTERNAL_API_CALL(expression, message) MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_COMPATIBLE_ALLOCATOR(expression, message)    MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_ARGUMENT_WITHIN_DOMAIN(expression, message)  MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_PEDANTIC(expression, message)                MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_SEMANTIC_REQUIREMENT(expression, message)    MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_INTERNAL(expression, message)                MSTD_ASSERT(expression, message)
#  define MSTD_ASSERT_UNCATEGORIZED(expression, message)           MSTD_ASSERT(expression, message)

// Disable all checks if hardening is not enabled.

#else

// All checks disabled.
#  define MSTD_ASSERT_VALID_INPUT_RANGE(expression, message)       ((void)0)
#  define MSTD_ASSERT_VALID_ELEMENT_ACCESS(expression, message)    ((void)0)
#  define MSTD_ASSERT_NON_NULL(expression, message)                ((void)0)
#  define MSTD_ASSERT_NON_OVERLAPPING_RANGES(expression, message)  ((void)0)
#  define MSTD_ASSERT_VALID_DEALLOCATION(expression, message)      ((void)0)
#  define MSTD_ASSERT_VALID_EXTERNAL_API_CALL(expression, message) ((void)0)
#  define MSTD_ASSERT_COMPATIBLE_ALLOCATOR(expression, message)    ((void)0)
#  define MSTD_ASSERT_ARGUMENT_WITHIN_DOMAIN(expression, message)  ((void)0)
#  define MSTD_ASSERT_PEDANTIC(expression, message)                ((void)0)
#  define MSTD_ASSERT_SEMANTIC_REQUIREMENT(expression, message)    ((void)0)
#  define MSTD_ASSERT_INTERNAL(expression, message)                ((void)0)
#  define MSTD_ASSERT_UNCATEGORIZED(expression, message)           ((void)0)

#endif // MSTD_HARDENING_MODE == MSTD_HARDENING_MODE_FAST
// clang-format on

#endif // MSTD_DETAIL_ASSERT_HPP