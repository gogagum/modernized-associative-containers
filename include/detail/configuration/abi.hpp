// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_CONFIGURATION_ABI_HPP
#define MSTD_CONFIGURATION_ABI_HPP

#include "compiler.hpp"
#include "platform.hpp"

#ifndef MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER
#  pragma GCC system_header
#endif

// FIXME: ABI detection should be done via compiler builtin macros. This
// is just a placeholder until Clang implements such macros. For now assume
// that Windows compilers pretending to be MSVC++ target the Microsoft ABI,
// and allow the user to explicitly specify the ABI to handle cases where this
// heuristic falls short.
#if MSTD_ABI_FORCE_ITANIUM && MSTD_ABI_FORCE_MICROSOFT
#  error "Only one of MSTD_ABI_FORCE_ITANIUM and MSTD_ABI_FORCE_MICROSOFT can be true"
#elif MSTD_ABI_FORCE_ITANIUM
#  define MSTD_ABI_ITANIUM
#elif MSTD_ABI_FORCE_MICROSOFT
#  define MSTD_ABI_MICROSOFT
#else
// Windows uses the Microsoft ABI
#  if defined(_WIN32) && defined(_MSC_VER)
#    define MSTD_ABI_MICROSOFT

// 32-bit ARM uses the Itanium ABI with a few differences (array cookies, etc),
// and so does 64-bit ARM on Apple platforms.
#  elif defined(__arm__) || (defined(__APPLE__) && defined(__aarch64__))
#    define MSTD_ABI_ITANIUM_WITH_ARM_DIFFERENCES

// Non-Apple 64-bit ARM uses the vanilla Itanium ABI
#  elif defined(__aarch64__)
#    define MSTD_ABI_ITANIUM

// We assume that other architectures also use the vanilla Itanium ABI too
#  else
#    define MSTD_ABI_ITANIUM
#  endif
#endif

#if MSTD_ABI_VERSION >= 2
// TODO: Move the description of the remaining ABI flags to ABIGuarantees.rst or remove them.

// Override the default return value of exception::what() for bad_function_call::what()
// with a string that is specific to bad_function_call (see http://wg21.link/LWG2233).
// This is an ABI break on platforms that sign and authenticate vtable function pointers
// because it changes the mangling of the virtual function located in the vtable, which
// changes how it gets signed.
#  define MSTD_ABI_BAD_FUNCTION_CALL_GOOD_WHAT_MESSAGE
// According to the Standard, `bitset::operator[] const` returns bool
#  define MSTD_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL

// These flags are documented in ABIGuarantees.rst
#  define MSTD_ABI_ALTERNATE_STRING_LAYOUT
#  define MSTD_ABI_ATOMIC_WAIT_NATIVE_BY_SIZE
#  define MSTD_ABI_DO_NOT_EXPORT_ALIGN
#  define MSTD_ABI_DO_NOT_EXPORT_BASIC_STRING_COMMON
#  define MSTD_ABI_DO_NOT_EXPORT_VECTOR_BASE_COMMON
#  define MSTD_ABI_DO_NOT_EXPORT_TO_CHARS_BASE_10
#  define MSTD_ABI_ENABLE_SHARED_PTR_TRIVIAL_ABI
#  define MSTD_ABI_ENABLE_UNIQUE_PTR_TRIVIAL_ABI
#  define MSTD_ABI_FIX_CITYHASH_IMPLEMENTATION
#  define MSTD_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE
#  define MSTD_ABI_INCOMPLETE_TYPES_IN_DEQUE
#  define MSTD_ABI_IOS_ALLOW_ARBITRARY_FILL_VALUE
#  define MSTD_ABI_NO_COMPRESSED_PAIR_PADDING
#  define MSTD_ABI_NO_FILESYSTEM_INLINE_NAMESPACE
#  define MSTD_ABI_NO_ITERATOR_BASES
#  define MSTD_ABI_NO_RANDOM_DEVICE_COMPATIBILITY_LAYOUT
#  define MSTD_ABI_NO_REVERSE_ITERATOR_SECOND_MEMBER
#  define MSTD_ABI_OPTIMIZED_FUNCTION
#  define MSTD_ABI_REGEX_CONSTANTS_NONZERO
#  define MSTD_ABI_STRING_OPTIMIZED_EXTERNAL_INSTANTIATION
#  define MSTD_ABI_USE_WRAP_ITER_IN_STD_ARRAY
#  define MSTD_ABI_USE_WRAP_ITER_IN_STD_STRING_VIEW
#  define MSTD_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION
#  define MSTD_ABI_TRIVIALLY_COPYABLE_BIT_ITERATOR

#elif MSTD_ABI_VERSION == 1
#  if !(defined(MSTD_OBJECT_FORMAT_COFF) || defined(MSTD_OBJECT_FORMAT_XCOFF))
// Enable compiling copies of now inline methods into the dylib to support
// applications compiled against older libraries. This is unnecessary with
// COFF dllexport semantics, since dllexport forces a non-inline definition
// of inline functions to be emitted anyway. Our own non-inline copy would
// conflict with the dllexport-emitted copy, so we disable it. For XCOFF,
// the linker will take issue with the symbols in the shared object if the
// weak inline methods get visibility (such as from -fvisibility-inlines-hidden),
// so disable it.
#    define MSTD_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS
#  endif
// Feature macros for disabling pre ABI v1 features. All of these options
// are deprecated.
#  if defined(__FreeBSD__)
#    define MSTD_DEPRECATED_ABI_DISABLE_PAIR_TRIVIAL_COPY_CTOR
#  endif
#endif

// TODO(LLVM 22): Remove this check
#if defined(MSTD_ABI_NO_ITERATOR_BASES) && !defined(MSTD_ABI_NO_REVERSE_ITERATOR_SECOND_MEMBER)
#  ifndef MSTD_ONLY_NO_ITERATOR_BASES
#    error "You probably want to define MSTD_ABI_NO_REVERSE_ITERATOR_SECOND_MEMBER. This has been split out from"   \
 " MSTD_ABI_NO_ITERATOR_BASES to allow only removing the second iterator member, since they aren't really related." \
 "If you actually want this ABI configuration, please define MSTD_ONLY_NO_ITERATOR_BASES instead."
#  endif
#endif

// We had some bugs where we use [[no_unique_address]] together with construct_at,
// which causes UB as the call on construct_at could write to overlapping subobjects
//
// https://llvm.org/PR70506
// https://llvm.org/PR70494
//
// To fix the bug we had to change the ABI of some classes to remove [[no_unique_address]] under certain conditions.
// The macro below is used for all classes whose ABI have changed as part of fixing these bugs.
#define MSTD_ABI_LLVM18_NO_UNIQUE_ADDRESS __attribute__((__abi_tag__("llvm18_nua")))

// [[msvc::no_unique_address]] seems to mostly affect empty classes, so the padding scheme for Itanium doesn't work.
#if defined(MSTD_ABI_MICROSOFT) && !defined(MSTD_ABI_NO_COMPRESSED_PAIR_PADDING)
#  define MSTD_ABI_NO_COMPRESSED_PAIR_PADDING
#endif

// Tracks the bounds of the array owned by std::unique_ptr<T[]>, allowing it to trap when accessed out-of-bounds.
// Note that limited bounds checking is also available outside of this ABI configuration, but only some categories
// of types can be checked.
//
// ABI impact: This causes the layout of std::unique_ptr<T[]> to change and its size to increase.
//             This also affects the representation of a few library types that use std::unique_ptr
//             internally, such as the unordered containers.
// #define MSTD_ABI_BOUNDED_UNIQUE_PTR

#if defined(MSTD_COMPILER_CLANG_BASED)
#  if defined(__APPLE__)
#    if defined(__i386__) || defined(__x86_64__)
// use old string layout on x86_64 and i386
#    elif defined(__arm__)
// use old string layout on arm (which does not include aarch64/arm64), except on watch ABIs
#      if defined(__ARM_ARCH_7K__) && __ARM_ARCH_7K__ >= 2
#        define MSTD_ABI_ALTERNATE_STRING_LAYOUT
#      endif
#    else
#      define MSTD_ABI_ALTERNATE_STRING_LAYOUT
#    endif
#  endif
#endif

#endif // MSTD_CONFIGURATION_ABI_HPP
