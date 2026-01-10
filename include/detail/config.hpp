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

#include "configuration/abi.hpp"
#include "configuration/availability.hpp"
#include "configuration/compiler.hpp"
//#include <__configuration/experimental.h>
//#include <__configuration/hardening.h>
#include "configuration/language.hpp"
#include "configuration/platform.hpp"

#ifndef MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER
#  pragma GCC system_header
#endif

#ifdef __cplusplus

// The attributes supported by clang are documented at https://clang.llvm.org/docs/AttributeReference.html

// MSTD_VERSION represents the version of libc++, which matches the version of LLVM.
// Given a LLVM release LLVM XX.YY.ZZ (e.g. LLVM 17.0.1 == 17.00.01), MSTD_VERSION is
// defined to XXYYZZ.
#  define MSTD_VERSION 220000

#  define MSTD_CONCAT_IMPL(_X, _Y) _X##_Y
#  define MSTD_CONCAT(_X, _Y) MSTD_CONCAT_IMPL(_X, _Y)
#  define MSTD_CONCAT3(X, Y, Z) MSTD_CONCAT(X, MSTD_CONCAT(Y, Z))

#  if __STDC_HOSTED__ == 0
#    define MSTD_FREESTANDING
#  endif

#  define MSTD_TOSTRING2(x) #x
#  define MSTD_TOSTRING(x) MSTD_TOSTRING2(x)

#  ifndef __has_constexpr_builtin
#    define __has_constexpr_builtin(x) 0
#  endif

// This checks whether a Clang module is built
#  ifndef __building_module
#    define __building_module(...) 0
#  endif

// '__is_identifier' returns '0' if '__x' is a reserved identifier provided by
// the compiler and '1' otherwise.
#  ifndef __is_identifier
#    define __is_identifier(__x) 1
#  endif

#  ifndef __has_declspec_attribute
#    define __has_declspec_attribute(__x) 0
#  endif

#  define __has_keyword(__x) !(__is_identifier(__x))

#  ifndef __has_warning
#    define __has_warning(...) 0
#  endif

#  if !defined(MSTD_COMPILER_CLANG_BASED) && __cplusplus < 201103L
#    error "libc++ only supports C++03 with Clang-based compilers. Please enable C++11"
#  endif

#  if defined(MSTD_ABI_MICROSOFT) && !defined(MSTD_NO_VCRUNTIME)
#    define MSTD_ABI_VCRUNTIME
#  endif

#  if defined(__MVS__)
#    include <features.h> // for __NATIVE_ASCII_F
#  endif

#  if defined(_WIN32)
#    define MSTD_WIN32API
#    define MSTD_SHORT_WCHAR 1
// Both MinGW and native MSVC provide a "MSVC"-like environment
#    define MSTD_MSVCRT_LIKE
// If mingw not explicitly detected, assume using MS C runtime only if
// a MS compatibility version is specified.
#    if defined(_MSC_VER) && !defined(__MINGW32__)
#      define MSTD_MSVCRT // Using Microsoft's C Runtime library
#    endif
#    if (defined(_M_AMD64) || defined(__x86_64__)) || (defined(_M_ARM) || defined(__arm__))
#      define MSTD_HAS_BITSCAN64 1
#    else
#      define MSTD_HAS_BITSCAN64 0
#    endif
#    define MSTD_HAS_OPEN_WITH_WCHAR 1
#  else
#    define MSTD_HAS_OPEN_WITH_WCHAR 0
#    define MSTD_HAS_BITSCAN64 0
#  endif // defined(_WIN32)

#  if defined(_AIX) && !defined(__64BIT__)
// The size of wchar is 2 byte on 32-bit mode on AIX.
#    define MSTD_SHORT_WCHAR 1
#  endif

// Libc++ supports various implementations of std::random_device.
//
// MSTD_USING_DEV_RANDOM
//      Read entropy from the given file, by default `/dev/urandom`.
//      If a token is provided, it is assumed to be the path to a file
//      to read entropy from. This is the default behavior if nothing
//      else is specified. This implementation requires storing state
//      inside `std::random_device`.
//
// MSTD_USING_ARC4_RANDOM
//      Use arc4random(). This allows obtaining random data even when
//      using sandboxing mechanisms. On some platforms like Apple, this
//      is the recommended source of entropy for user-space programs.
//      When this option is used, the token passed to `std::random_device`'s
//      constructor *must* be "/dev/urandom" -- anything else is an error.
//
// MSTD_USING_GETENTROPY
//      Use getentropy().
//      When this option is used, the token passed to `std::random_device`'s
//      constructor *must* be "/dev/urandom" -- anything else is an error.
//
// MSTD_USING_FUCHSIA_CPRNG
//      Use Fuchsia's zx_cprng_draw() system call, which is specified to
//      deliver high-quality entropy and cannot fail.
//      When this option is used, the token passed to `std::random_device`'s
//      constructor *must* be "/dev/urandom" -- anything else is an error.
//
// MSTD_USING_WIN32_RANDOM
//      Use rand_s(), for use on Windows.
//      When this option is used, the token passed to `std::random_device`'s
//      constructor *must* be "/dev/urandom" -- anything else is an error.
#  if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||                     \
      defined(__DragonFly__)
#    define MSTD_USING_ARC4_RANDOM
#  elif defined(__wasi__) || defined(__EMSCRIPTEN__)
#    define MSTD_USING_GETENTROPY
#  elif defined(__Fuchsia__)
#    define MSTD_USING_FUCHSIA_CPRNG
#  elif defined(MSTD_WIN32API)
#    define MSTD_USING_WIN32_RANDOM
#  else
#    define MSTD_USING_DEV_RANDOM
#  endif

#    define MSTD_ALIGNOF(...) alignof(__VA_ARGS__)
#    define _ALIGNAS_TYPE(x) alignas(x)
#    define _ALIGNAS(x) alignas(x)
#    define _NOEXCEPT noexcept
#    define _NOEXCEPT_(...) noexcept(__VA_ARGS__)
#    define MSTD_CONSTEXPR constexpr

#  define MSTD_PREFERRED_ALIGNOF(_Tp) __alignof(_Tp)

#  if __has_extension(blocks) && defined(__APPLE__)
#    define MSTD_HAS_BLOCKS_RUNTIME 1
#  else
#    define MSTD_HAS_BLOCKS_RUNTIME 0
#  endif

#  define MSTD_ALWAYS_INLINE __attribute__((__always_inline__))

#  if defined(MSTD_OBJECT_FORMAT_COFF)

#    ifdef _DLL
#      define MSTD_CRT_FUNC __declspec(dllimport)
#    else
#      define MSTD_CRT_FUNC
#    endif

#    if defined(MSTD_DISABLE_VISIBILITY_ANNOTATIONS) || (defined(__MINGW32__) && !defined(MSTD_BUILDING_LIBRARY))
#      define MSTD_EXTERN_TEMPLATE_TYPE_VIS
#      define MSTD_CLASS_TEMPLATE_INSTANTIATION_VIS
#      define MSTD_OVERRIDABLE_FUNC_VIS
#      define MSTD_EXPORTED_FROM_ABI
#    elif defined(MSTD_BUILDING_LIBRARY)
#      if defined(__MINGW32__)
#        define MSTD_EXTERN_TEMPLATE_TYPE_VIS __declspec(dllexport)
#        define MSTD_CLASS_TEMPLATE_INSTANTIATION_VIS
#      else
#        define MSTD_EXTERN_TEMPLATE_TYPE_VIS
#        define MSTD_CLASS_TEMPLATE_INSTANTIATION_VIS __declspec(dllexport)
#      endif
#      define MSTD_OVERRIDABLE_FUNC_VIS __declspec(dllexport)
#      define MSTD_EXPORTED_FROM_ABI __declspec(dllexport)
#    else
#      define MSTD_EXTERN_TEMPLATE_TYPE_VIS __declspec(dllimport)
#      define MSTD_CLASS_TEMPLATE_INSTANTIATION_VIS
#      define MSTD_OVERRIDABLE_FUNC_VIS
#      define MSTD_EXPORTED_FROM_ABI __declspec(dllimport)
#    endif

#    define MSTD_HIDDEN
#    define MSTD_TEMPLATE_DATA_VIS
#    define MSTD_NAMESPACE_VISIBILITY

#  else

#    if !defined(MSTD_DISABLE_VISIBILITY_ANNOTATIONS)
#      define MSTD_VISIBILITY(vis) __attribute__((__visibility__(vis)))
#    else
#      define MSTD_VISIBILITY(vis)
#    endif

#    define MSTD_HIDDEN MSTD_VISIBILITY("hidden")
#    define MSTD_TEMPLATE_DATA_VIS MSTD_VISIBILITY("default")
#    define MSTD_EXPORTED_FROM_ABI MSTD_VISIBILITY("default")
#    define MSTD_EXTERN_TEMPLATE_TYPE_VIS MSTD_VISIBILITY("default")
#    define MSTD_CLASS_TEMPLATE_INSTANTIATION_VIS

// TODO: Make this a proper customization point or remove the option to override it.
#    ifndef MSTD_OVERRIDABLE_FUNC_VIS
#      define MSTD_OVERRIDABLE_FUNC_VIS MSTD_VISIBILITY("default")
#    endif

#    if !defined(MSTD_DISABLE_VISIBILITY_ANNOTATIONS) && __has_attribute(__type_visibility__)
#      define MSTD_NAMESPACE_VISIBILITY __attribute__((__type_visibility__("default")))
#    elif !defined(MSTD_DISABLE_VISIBILITY_ANNOTATIONS)
#      define MSTD_NAMESPACE_VISIBILITY __attribute__((__visibility__("default")))
#    else
#      define MSTD_NAMESPACE_VISIBILITY
#    endif

#  endif // defined(MSTD_OBJECT_FORMAT_COFF)

#  if __has_attribute(exclude_from_explicit_instantiation)
#    define MSTD_EXCLUDE_FROM_EXPLICIT_INSTANTIATION __attribute__((__exclude_from_explicit_instantiation__))
#  else
// Try to approximate the effect of exclude_from_explicit_instantiation
// (which is that entities are not assumed to be provided by explicit
// template instantiations in the dylib) by always inlining those entities.
#    define MSTD_EXCLUDE_FROM_EXPLICIT_INSTANTIATION MSTD_ALWAYS_INLINE
#  endif

#  ifdef MSTD_COMPILER_CLANG_BASED
#    define MSTD_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
#    define MSTD_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#    define MSTD_CLANG_DIAGNOSTIC_IGNORED(str) _Pragma(MSTD_TOSTRING(clang diagnostic ignored str))
#    define MSTD_GCC_DIAGNOSTIC_IGNORED(str)
#  elif defined(MSTD_COMPILER_GCC)
#    define MSTD_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#    define MSTD_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#    define MSTD_CLANG_DIAGNOSTIC_IGNORED(str)
#    define MSTD_GCC_DIAGNOSTIC_IGNORED(str) _Pragma(MSTD_TOSTRING(GCC diagnostic ignored str))
#  else
#    define MSTD_DIAGNOSTIC_PUSH
#    define MSTD_DIAGNOSTIC_POP
#    define MSTD_CLANG_DIAGNOSTIC_IGNORED(str)
#    define MSTD_GCC_DIAGNOSTIC_IGNORED(str)
#  endif

// Macros to enter and leave a state where deprecation warnings are suppressed.
#  define MSTD_SUPPRESS_DEPRECATED_PUSH                                                                             \
    MSTD_DIAGNOSTIC_PUSH MSTD_CLANG_DIAGNOSTIC_IGNORED("-Wdeprecated")                                           \
        MSTD_GCC_DIAGNOSTIC_IGNORED("-Wdeprecated-declarations")
#  define MSTD_SUPPRESS_DEPRECATED_POP MSTD_DIAGNOSTIC_POP

#  if MSTD_HARDENING_MODE == MSTD_HARDENING_MODE_FAST
#    define MSTD_HARDENING_SIG f
#  elif MSTD_HARDENING_MODE == MSTD_HARDENING_MODE_EXTENSIVE
#    define MSTD_HARDENING_SIG s
#  elif MSTD_HARDENING_MODE == MSTD_HARDENING_MODE_DEBUG
#    define MSTD_HARDENING_SIG d
#  else
#    define MSTD_HARDENING_SIG n // "none"
#  endif

#  if MSTD_ASSERTION_SEMANTIC == MSTD_ASSERTION_SEMANTIC_OBSERVE
#    define MSTD_ASSERTION_SEMANTIC_SIG o
#  elif MSTD_ASSERTION_SEMANTIC == MSTD_ASSERTION_SEMANTIC_QUICK_ENFORCE
#    define MSTD_ASSERTION_SEMANTIC_SIG q
#  elif MSTD_ASSERTION_SEMANTIC == MSTD_ASSERTION_SEMANTIC_ENFORCE
#    define MSTD_ASSERTION_SEMANTIC_SIG e
#  else
#    define MSTD_ASSERTION_SEMANTIC_SIG i // `ignore`
#  endif

#  if !MSTD_HAS_EXCEPTIONS
#    define MSTD_EXCEPTIONS_SIG n
#  else
#    define MSTD_EXCEPTIONS_SIG e
#  endif

#  define MSTD_ODR_SIGNATURE                                                                                        \
    MSTD_CONCAT(                                                                                                    \
        MSTD_CONCAT(MSTD_CONCAT(MSTD_HARDENING_SIG, MSTD_ASSERTION_SEMANTIC_SIG), MSTD_EXCEPTIONS_SIG), \
        MSTD_VERSION)

// This macro marks a symbol as being hidden from libc++'s ABI. This is achieved
// on two levels:
// 1. The symbol is given hidden visibility, which ensures that users won't start exporting
//    symbols from their dynamic library by means of using the libc++ headers. This ensures
//    that those symbols stay private to the dynamic library in which it is defined.
//
// 2. The symbol is given an ABI tag that encodes the ODR-relevant properties of the library.
//    This ensures that no ODR violation can arise from mixing two TUs compiled with different
//    versions or configurations of libc++ (such as exceptions vs no-exceptions). Indeed, if the
//    program contains two definitions of a function, the ODR requires them to be token-by-token
//    equivalent, and the linker is allowed to pick either definition and discard the other one.
//
//    For example, if a program contains a copy of `vector::at()` compiled with exceptions enabled
//    *and* a copy of `vector::at()` compiled with exceptions disabled (by means of having two TUs
//    compiled with different settings), the two definitions are both visible by the linker and they
//    have the same name, but they have a meaningfully different implementation (one throws an exception
//    and the other aborts the program). This violates the ODR and makes the program ill-formed, and in
//    practice what will happen is that the linker will pick one of the definitions at random and will
//    discard the other one. This can quite clearly lead to incorrect program behavior.
//
//    A similar reasoning holds for many other properties that are ODR-affecting. Essentially any
//    property that causes the code of a function to differ from the code in another configuration
//    can be considered ODR-affecting. In practice, we don't encode all such properties in the ABI
//    tag, but we encode the ones that we think are most important: library version, exceptions, and
//    hardening mode.
//
//    Note that historically, solving this problem has been achieved in various ways, including
//    force-inlining all functions or giving internal linkage to all functions. Both these previous
//    solutions suffer from drawbacks that lead notably to code bloat.
//
// Note that we use MSTD_EXCLUDE_FROM_EXPLICIT_INSTANTIATION to ensure that we don't depend
// on MSTD_HIDE_FROM_ABI methods of classes explicitly instantiated in the dynamic library.
//
// Also note that the MSTD_HIDE_FROM_ABI_VIRTUAL macro should be used on virtual functions
// instead of MSTD_HIDE_FROM_ABI. That macro does not use an ABI tag. Indeed, the mangled
// name of a virtual function is part of its ABI, since some architectures like arm64e can sign
// the virtual function pointer in the vtable based on the mangled name of the function. Since
// we use an ABI tag that changes with each released version, the mangled name of the virtual
// function would change, which is incorrect. Note that it doesn't make much sense to change
// the implementation of a virtual function in an ABI-incompatible way in the first place,
// since that would be an ABI break anyway. Hence, the lack of ABI tag should not be noticeable.
//
// The macro can be applied to record and enum types. When the tagged type is nested in
// a record this "parent" record needs to have the macro too. Another use case for applying
// this macro to records and unions is to apply an ABI tag to inline constexpr variables.
// This can be useful for inline variables that are implementation details which are expected
// to change in the future.
//
// TODO: We provide a escape hatch with MSTD_NO_ABI_TAG for folks who want to avoid increasing
//       the length of symbols with an ABI tag. In practice, we should remove the escape hatch and
//       use compression mangling instead, see https://github.com/itanium-cxx-abi/cxx-abi/issues/70.
#  ifndef MSTD_NO_ABI_TAG
#    define MSTD_HIDE_FROM_ABI                                                                                      \
      MSTD_HIDDEN MSTD_EXCLUDE_FROM_EXPLICIT_INSTANTIATION                                                       \
      __attribute__((__abi_tag__(MSTD_TOSTRING(MSTD_ODR_SIGNATURE))))
#  else
#    define MSTD_HIDE_FROM_ABI MSTD_HIDDEN MSTD_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#  endif
#  define MSTD_HIDE_FROM_ABI_VIRTUAL MSTD_HIDDEN MSTD_EXCLUDE_FROM_EXPLICIT_INSTANTIATION

#  ifdef MSTD_BUILDING_LIBRARY
#    if MSTD_ABI_VERSION > 1
#      define MSTD_HIDE_FROM_ABI_AFTER_V1 MSTD_HIDE_FROM_ABI
#    else
#      define MSTD_HIDE_FROM_ABI_AFTER_V1
#    endif
#  else
#    define MSTD_HIDE_FROM_ABI_AFTER_V1 MSTD_HIDE_FROM_ABI
#  endif

// Clang modules take a significant compile time hit when pushing and popping diagnostics.
// Since all the headers are marked as system headers unless MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER is defined, we can
// simply disable this pushing and popping when MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER isn't defined.
#  ifdef MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER
#    define MSTD_PUSH_EXTENSION_DIAGNOSTICS                                                                         \
      MSTD_DIAGNOSTIC_PUSH                                                                                          \
      MSTD_CLANG_DIAGNOSTIC_IGNORED("-Wc++11-extensions")                                                           \
      MSTD_CLANG_DIAGNOSTIC_IGNORED("-Wc++14-extensions")                                                           \
      MSTD_CLANG_DIAGNOSTIC_IGNORED("-Wc++17-extensions")                                                           \
      MSTD_CLANG_DIAGNOSTIC_IGNORED("-Wc++20-extensions")                                                           \
      MSTD_CLANG_DIAGNOSTIC_IGNORED("-Wc++23-extensions")                                                           \
      MSTD_GCC_DIAGNOSTIC_IGNORED("-Wc++14-extensions")                                                             \
      MSTD_GCC_DIAGNOSTIC_IGNORED("-Wc++17-extensions")                                                             \
      MSTD_GCC_DIAGNOSTIC_IGNORED("-Wc++20-extensions")                                                             \
      MSTD_GCC_DIAGNOSTIC_IGNORED("-Wc++23-extensions")
#    define MSTD_POP_EXTENSION_DIAGNOSTICS MSTD_DIAGNOSTIC_POP
#  else
#    define MSTD_PUSH_EXTENSION_DIAGNOSTICS
#    define MSTD_POP_EXTENSION_DIAGNOSTICS
#  endif

// clang-format off

// The unversioned namespace is used when we want to be ABI compatible with other standard libraries in some way. There
// are two main categories where that's the case:
// - Historically, we have made exception types ABI compatible with libstdc++ to allow throwing them between libstdc++
//   and libc++. This is not used anymore for new exception types, since there is no use-case for it anymore.
// - Types and functions which are used by the compiler are in the unversioned namespace, since the compiler has to know
//   their mangling without the appropriate declaration in some cases.
// If it's not clear whether using the unversioned namespace is the correct thing to do, it's not. The versioned
// namespace (MSTD_BEGIN_NAMESPACE_STD) should almost always be used.
#  define MSTD_BEGIN_UNVERSIONED_NAMESPACE_STD                                                                      \
    MSTD_PUSH_EXTENSION_DIAGNOSTICS namespace MSTD_NAMESPACE_VISIBILITY mstd {

#  define MSTD_END_UNVERSIONED_NAMESPACE_STD } MSTD_POP_EXTENSION_DIAGNOSTICS

#  define MSTD_BEGIN_NAMESPACE_STD MSTD_BEGIN_UNVERSIONED_NAMESPACE_STD inline namespace MSTD_ABI_NAMESPACE {
#  define MSTD_END_NAMESPACE } MSTD_END_UNVERSIONED_NAMESPACE_STD

// TODO: This should really be in the versioned namespace
#define MSTD_BEGIN_NAMESPACE_EXPERIMENTAL MSTD_BEGIN_UNVERSIONED_NAMESPACE_STD namespace experimental {
#define MSTD_END_NAMESPACE_EXPERIMENTAL } MSTD_END_UNVERSIONED_NAMESPACE_STD

#define MSTD_BEGIN_NAMESPACE_LFTS MSTD_BEGIN_NAMESPACE_EXPERIMENTAL inline namespace fundamentals_v1 {
#define MSTD_END_NAMESPACE_LFTS } MSTD_END_NAMESPACE_EXPERIMENTAL

#define MSTD_BEGIN_NAMESPACE_LFTS_V2 MSTD_BEGIN_NAMESPACE_EXPERIMENTAL inline namespace fundamentals_v2 {
#define MSTD_END_NAMESPACE_LFTS_V2 } MSTD_END_NAMESPACE_EXPERIMENTAL

#ifdef MSTD_ABI_NO_FILESYSTEM_INLINE_NAMESPACE
#  define MSTD_BEGIN_NAMESPACE_FILESYSTEM MSTD_BEGIN_NAMESPACE_STD namespace filesystem {
#  define MSTD_END_NAMESPACE_FILESYSTEM } MSTD_END_NAMESPACE
#else
#  define MSTD_BEGIN_NAMESPACE_FILESYSTEM MSTD_BEGIN_NAMESPACE_STD                                               \
                                             inline namespace __fs { namespace filesystem {

#  define MSTD_END_NAMESPACE_FILESYSTEM }} MSTD_END_NAMESPACE
#endif

// clang-format on

#  if __has_attribute(__enable_if__)
#    define MSTD_PREFERRED_OVERLOAD __attribute__((__enable_if__(true, "")))
#  endif

#  if !defined(__SIZEOF_INT128__) || defined(_MSC_VER)
#    define MSTD_HAS_INT128 0
#  else
#    define MSTD_HAS_INT128 1
#  endif

#    define MSTD_DECLARE_STRONG_ENUM(x) enum class x
#    define MSTD_DECLARE_STRONG_ENUM_EPILOG(x)

#  ifdef __FreeBSD__
#    define _DECLARE_C99_LDBL_MATH 1
#  endif

// If we are getting operator new from the MSVC CRT, then allocation overloads
// for align_val_t were added in 19.12, aka VS 2017 version 15.3.
#  if defined(MSTD_MSVCRT) && defined(_MSC_VER) && _MSC_VER < 1912
#    define MSTD_HAS_LIBRARY_ALIGNED_ALLOCATION 0
#  elif defined(MSTD_ABI_VCRUNTIME) && !defined(__cpp_aligned_new)
// We're deferring to Microsoft's STL to provide aligned new et al. We don't
// have it unless the language feature test macro is defined.
#    define MSTD_HAS_LIBRARY_ALIGNED_ALLOCATION 0
#  elif defined(__MVS__)
#    define MSTD_HAS_LIBRARY_ALIGNED_ALLOCATION 0
#  else
#    define MSTD_HAS_LIBRARY_ALIGNED_ALLOCATION 1
#  endif

#  if !MSTD_HAS_LIBRARY_ALIGNED_ALLOCATION || (!defined(__cpp_aligned_new) || __cpp_aligned_new < 201606)
#    define MSTD_HAS_ALIGNED_ALLOCATION 0
#  else
#    define MSTD_HAS_ALIGNED_ALLOCATION 1
#  endif

#  if defined(__APPLE__) || defined(__FreeBSD__)
#    define MSTD_WCTYPE_IS_MASK
#  endif

#  if MSTD_STD_VER <= 17 || !defined(__cpp_char8_t)
#    define MSTD_HAS_CHAR8_T 0
#  else
#    define MSTD_HAS_CHAR8_T 1
#  endif

// Deprecation macros.
//
// Deprecations warnings are always enabled, except when users explicitly opt-out
// by defining MSTD_DISABLE_DEPRECATION_WARNINGS.

#define MSTD_DEPRECATED [[deprecated]]
#define MSTD_DEPRECATED_(m) [[deprecated(m)]]

// FIXME: using `#warning` causes diagnostics from system headers which include deprecated headers. This can only be
// enabled again once https://github.com/llvm/llvm-project/pull/168041 (or a similar feature) has landed, since that
// allows suppression in system headers.
#  if defined(__DEPRECATED) && __DEPRECATED && !defined(MSTD_DISABLE_DEPRECATION_WARNINGS) && 0
#    define MSTD_DIAGNOSE_DEPRECATED_HEADERS 1
#  else
#    define MSTD_DIAGNOSE_DEPRECATED_HEADERS 0
#  endif

#define MSTD_DEPRECATED_IN_CXX11 MSTD_DEPRECATED
#define MSTD_DEPRECATED_IN_CXX14 MSTD_DEPRECATED
#define MSTD_DEPRECATED_IN_CXX17 MSTD_DEPRECATED
#define MSTD_DEPRECATED_IN_CXX20 MSTD_DEPRECATED
#define MSTD_DEPRECATED_IN_CXX23 MSTD_DEPRECATED

#  if MSTD_STD_VER >= 26
#    define MSTD_DEPRECATED_IN_CXX26 MSTD_DEPRECATED
#    define MSTD_DEPRECATED_IN_CXX26_(m) MSTD_DEPRECATED_(m)
#  else
#    define MSTD_DEPRECATED_IN_CXX26
#    define MSTD_DEPRECATED_IN_CXX26_(m)
#  endif

#  if MSTD_HAS_CHAR8_T
#    define MSTD_DEPRECATED_WITH_CHAR8_T MSTD_DEPRECATED
#  else
#    define MSTD_DEPRECATED_WITH_CHAR8_T
#  endif

#define MSTD_EXPLICIT_SINCE_CXX14 explicit
#define MSTD_EXPLICIT_SINCE_CXX23 explicit
#define MSTD_CONSTEXPR_SINCE_CXX14 constexpr
#define MSTD_CONSTEXPR_SINCE_CXX17 constexpr
#define MSTD_CONSTEXPR_SINCE_CXX20 constexpr
#define MSTD_CONSTEXPR_SINCE_CXX23 constexpr

#  if MSTD_STD_VER >= 26
#    define MSTD_CONSTEXPR_SINCE_CXX26 constexpr
#  else
#    define MSTD_CONSTEXPR_SINCE_CXX26
#  endif

#  ifndef MSTD_WEAK
#    define MSTD_WEAK __attribute__((__weak__))
#  endif

// Thread API
// clang-format off
#  if MSTD_HAS_THREADS &&                                                                                           \
      !MSTD_HAS_THREAD_API_PTHREAD &&                                                                               \
      !MSTD_HAS_THREAD_API_WIN32 &&                                                                                 \
      !MSTD_HAS_THREAD_API_EXTERNAL

#    if defined(__FreeBSD__) ||                                                                                        \
        defined(__wasi__) ||                                                                                           \
        defined(__NetBSD__) ||                                                                                         \
        defined(__OpenBSD__) ||                                                                                        \
        defined(__NuttX__) ||                                                                                          \
        defined(__linux__) ||                                                                                          \
        defined(__GNU__) ||                                                                                            \
        defined(__APPLE__) ||                                                                                          \
        defined(__MVS__) ||                                                                                            \
        defined(_AIX) ||                                                                                               \
        defined(__EMSCRIPTEN__)
// clang-format on
#      undef MSTD_HAS_THREAD_API_PTHREAD
#      define MSTD_HAS_THREAD_API_PTHREAD 1
#    elif defined(__Fuchsia__)
// TODO(44575): Switch to C11 thread API when possible.
#      undef MSTD_HAS_THREAD_API_PTHREAD
#      define MSTD_HAS_THREAD_API_PTHREAD 1
#    elif defined(MSTD_WIN32API)
#      undef MSTD_HAS_THREAD_API_WIN32
#      define MSTD_HAS_THREAD_API_WIN32 1
#    else
#      error "No thread API"
#    endif // MSTD_HAS_THREAD_API
#  endif   // MSTD_HAS_THREADS

#  if !MSTD_HAS_THREAD_API_PTHREAD
#    define MSTD_HAS_COND_CLOCKWAIT 0
#  elif (defined(__ANDROID__) && __ANDROID_API__ >= 30) || MSTD_GLIBC_PREREQ(2, 30)
#    define MSTD_HAS_COND_CLOCKWAIT 1
#  else
#    define MSTD_HAS_COND_CLOCKWAIT 0
#  endif

#  if !MSTD_HAS_THREADS && MSTD_HAS_THREAD_API_PTHREAD
#    error MSTD_HAS_THREAD_API_PTHREAD may only be true when MSTD_HAS_THREADS is true.
#  endif

#  if !MSTD_HAS_THREADS && MSTD_HAS_THREAD_API_EXTERNAL
#    error MSTD_HAS_THREAD_API_EXTERNAL may only be true when MSTD_HAS_THREADS is true.
#  endif

#  if !MSTD_HAS_MONOTONIC_CLOCK && MSTD_HAS_THREADS
#    error MSTD_HAS_MONOTONIC_CLOCK may only be false when MSTD_HAS_THREADS is false.
#  endif

#  if MSTD_HAS_THREADS && !defined(__STDCPP_THREADS__)
#    define __STDCPP_THREADS__ 1
#  endif

// The glibc and Bionic implementation of pthreads implements
// pthread_mutex_destroy as nop for regular mutexes. Additionally, Win32
// mutexes have no destroy mechanism.
//
// This optimization can't be performed on Apple platforms, where
// pthread_mutex_destroy can allow the kernel to release resources.
// See https://llvm.org/D64298 for details.
//
// TODO(EricWF): Enable this optimization on Bionic after speaking to their
//               respective stakeholders.
// clang-format off
#  if (MSTD_HAS_THREAD_API_PTHREAD && defined(__GLIBC__)) ||                                                        \
      (MSTD_HAS_THREAD_API_C11 && defined(__Fuchsia__)) ||                                                          \
       MSTD_HAS_THREAD_API_WIN32
// clang-format on
#    define MSTD_HAS_TRIVIAL_MUTEX_DESTRUCTION 1
#  else
#    define MSTD_HAS_TRIVIAL_MUTEX_DESTRUCTION 0
#  endif

// Destroying a condvar is a nop on Windows.
//
// This optimization can't be performed on Apple platforms, where
// pthread_cond_destroy can allow the kernel to release resources.
// See https://llvm.org/D64298 for details.
//
// TODO(EricWF): This is potentially true for some pthread implementations
// as well.
#  if (MSTD_HAS_THREAD_API_C11 && defined(__Fuchsia__)) || MSTD_HAS_THREAD_API_WIN32
#    define MSTD_HAS_TRIVIAL_CONDVAR_DESTRUCTION 1
#  else
#    define MSTD_HAS_TRIVIAL_CONDVAR_DESTRUCTION 0
#  endif

#  if defined(__BIONIC__) || defined(__NuttX__) || defined(__Fuchsia__) || defined(__wasi__) ||                        \
      MSTD_HAS_MUSL_LIBC || defined(__OpenBSD__) || defined(__LLVM_LIBC__)
#    define MSTD_PROVIDES_DEFAULT_RUNE_TABLE
#  endif

#  if __has_feature(cxx_atomic) || __has_extension(c_atomic) || __has_keyword(_Atomic)
#    define MSTD_HAS_C_ATOMIC_IMP 1
#    define MSTD_HAS_GCC_ATOMIC_IMP 0
#    define MSTD_HAS_EXTERNAL_ATOMIC_IMP 0
#  elif defined(MSTD_COMPILER_GCC)
#    define MSTD_HAS_C_ATOMIC_IMP 0
#    define MSTD_HAS_GCC_ATOMIC_IMP 1
#    define MSTD_HAS_EXTERNAL_ATOMIC_IMP 0
#  endif

#  if !MSTD_HAS_C_ATOMIC_IMP && !MSTD_HAS_GCC_ATOMIC_IMP && !MSTD_HAS_EXTERNAL_ATOMIC_IMP
#    define MSTD_HAS_ATOMIC_HEADER 0
#  else
#    define MSTD_HAS_ATOMIC_HEADER 1
#    ifndef MSTD_ATOMIC_FLAG_TYPE
#      define MSTD_ATOMIC_FLAG_TYPE bool
#    endif
#  endif

#  if __has_cpp_attribute(_Clang::__no_thread_safety_analysis__)
#    define MSTD_NO_THREAD_SAFETY_ANALYSIS [[_Clang::__no_thread_safety_analysis__]]
#  else
#    define MSTD_NO_THREAD_SAFETY_ANALYSIS
#  endif

// TODO: remove MSTD_CONSTINIT
#define MSTD_CONSTINIT constinit

#  if defined(__CUDACC__) || defined(__CUDA_ARCH__) || defined(__CUDA_LIBDEVICE__)
// The CUDA SDK contains an unfortunate definition for the __noinline__ macro,
// which breaks the regular __attribute__((__noinline__)) syntax. Therefore,
// when compiling for CUDA we use the non-underscored version of the noinline
// attribute.
//
// This is a temporary workaround and we still expect the CUDA SDK team to solve
// this issue properly in the SDK headers.
//
// See https://github.com/llvm/llvm-project/pull/73838 for more details.
#    define MSTD_NOINLINE __attribute__((noinline))
#  elif __has_attribute(__noinline__)
#    define MSTD_NOINLINE __attribute__((__noinline__))
#  else
#    define MSTD_NOINLINE
#  endif

// We often repeat things just for handling wide characters in the library.
// When wide characters are disabled, it can be useful to have a quick way of
// disabling it without having to resort to #if-#endif, which has a larger
// impact on readability.
#  if !MSTD_HAS_WIDE_CHARACTERS
#    define MSTD_IF_WIDE_CHARACTERS(...)
#  else
#    define MSTD_IF_WIDE_CHARACTERS(...) __VA_ARGS__
#  endif

// clang-format off
#  define MSTD_PUSH_MACROS _Pragma("push_macro(\"min\")") _Pragma("push_macro(\"max\")") _Pragma("push_macro(\"refresh\")") _Pragma("push_macro(\"move\")") _Pragma("push_macro(\"erase\")")
#  define MSTD_POP_MACROS _Pragma("pop_macro(\"min\")") _Pragma("pop_macro(\"max\")") _Pragma("pop_macro(\"refresh\")") _Pragma("pop_macro(\"move\")") _Pragma("pop_macro(\"erase\")")
// clang-format on

#  ifndef MSTD_NO_AUTO_LINK
#    if defined(MSTD_ABI_MICROSOFT) && !defined(MSTD_BUILDING_LIBRARY)
#      if !defined(MSTD_DISABLE_VISIBILITY_ANNOTATIONS)
#        pragma comment(lib, "c++.lib")
#      else
#        pragma comment(lib, "libc++.lib")
#      endif
#    endif // defined(MSTD_ABI_MICROSOFT) && !defined(MSTD_BUILDING_LIBRARY)
#  endif   // MSTD_NO_AUTO_LINK

// Configures the fopen close-on-exec mode character, if any. This string will
// be appended to any mode string used by fstream for fopen/fdopen.
//
// Not all platforms support this, but it helps avoid fd-leaks on platforms that
// do.
#  if defined(__BIONIC__)
#    define MSTD_FOPEN_CLOEXEC_MODE "e"
#  else
#    define MSTD_FOPEN_CLOEXEC_MODE
#  endif

#  if __has_cpp_attribute(msvc::no_unique_address)
// MSVC implements [[no_unique_address]] as a silent no-op currently.
// (If/when MSVC breaks its C++ ABI, it will be changed to work as intended.)
// However, MSVC implements [[msvc::no_unique_address]] which does what
// [[no_unique_address]] is supposed to do, in general.
#    define MSTD_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#  else
#    define MSTD_NO_UNIQUE_ADDRESS [[__no_unique_address__]]
#  endif

// c8rtomb() and mbrtoc8() were added in C++20 and C23. Support for these
// functions is gradually being added to existing C libraries. The conditions
// below check for known C library versions and conditions under which these
// functions are declared by the C library.
//
// GNU libc 2.36 and newer declare c8rtomb() and mbrtoc8() in C++ modes if
// __cpp_char8_t is defined or if C2X extensions are enabled. Determining
// the latter depends on internal GNU libc details that are not appropriate
// to depend on here, so any declarations present when __cpp_char8_t is not
// defined are ignored.
#  if MSTD_GLIBC_PREREQ(2, 36) && defined(__cpp_char8_t)
#    define MSTD_HAS_C8RTOMB_MBRTOC8 1
#  else
#    define MSTD_HAS_C8RTOMB_MBRTOC8 0
#  endif

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

#  if defined(__OBJC__) && defined(MSTD_APPLE_CLANG_VER)
#    define MSTD_WORKAROUND_OBJCXX_COMPILER_INTRINSICS
#  endif

#  define _PSTL_PRAGMA(x) _Pragma(#x)

// Enable SIMD for compilers that support OpenMP 4.0
#  if (defined(_OPENMP) && _OPENMP >= 201307)

#    define _PSTL_UDR_PRESENT
#    define _PSTL_PRAGMA_SIMD _PSTL_PRAGMA(omp simd)
#    define _PSTL_PRAGMA_DECLARE_SIMD _PSTL_PRAGMA(omp declare simd)
#    define _PSTL_PRAGMA_SIMD_REDUCTION(PRM) _PSTL_PRAGMA(omp simd reduction(PRM))
#    define _PSTL_PRAGMA_SIMD_SCAN(PRM) _PSTL_PRAGMA(omp simd reduction(inscan, PRM))
#    define _PSTL_PRAGMA_SIMD_INCLUSIVE_SCAN(PRM) _PSTL_PRAGMA(omp scan inclusive(PRM))
#    define _PSTL_PRAGMA_SIMD_EXCLUSIVE_SCAN(PRM) _PSTL_PRAGMA(omp scan exclusive(PRM))

// Declaration of reduction functor, where
// NAME - the name of the functor
// OP - type of the callable object with the reduction operation
// omp_in - refers to the local partial result
// omp_out - refers to the final value of the combiner operator
// omp_priv - refers to the private copy of the initial value
// omp_orig - refers to the original variable to be reduced
#    define _PSTL_PRAGMA_DECLARE_REDUCTION(NAME, OP)                                                                   \
      _PSTL_PRAGMA(omp declare reduction(NAME:OP : omp_out(omp_in)) initializer(omp_priv = omp_orig))

#  elif defined(MSTD_COMPILER_CLANG_BASED)

#    define _PSTL_PRAGMA_SIMD _Pragma("clang loop vectorize(enable) interleave(enable)")
#    define _PSTL_PRAGMA_DECLARE_SIMD
#    define _PSTL_PRAGMA_SIMD_REDUCTION(PRM) _Pragma("clang loop vectorize(enable) interleave(enable)")
#    define _PSTL_PRAGMA_SIMD_SCAN(PRM) _Pragma("clang loop vectorize(enable) interleave(enable)")
#    define _PSTL_PRAGMA_SIMD_INCLUSIVE_SCAN(PRM)
#    define _PSTL_PRAGMA_SIMD_EXCLUSIVE_SCAN(PRM)
#    define _PSTL_PRAGMA_DECLARE_REDUCTION(NAME, OP)

#  else // (defined(_OPENMP) && _OPENMP >= 201307)

#    define _PSTL_PRAGMA_SIMD
#    define _PSTL_PRAGMA_DECLARE_SIMD
#    define _PSTL_PRAGMA_SIMD_REDUCTION(PRM)
#    define _PSTL_PRAGMA_SIMD_SCAN(PRM)
#    define _PSTL_PRAGMA_SIMD_INCLUSIVE_SCAN(PRM)
#    define _PSTL_PRAGMA_SIMD_EXCLUSIVE_SCAN(PRM)
#    define _PSTL_PRAGMA_DECLARE_REDUCTION(NAME, OP)

#  endif // (defined(_OPENMP) && _OPENMP >= 201307)

#  define _PSTL_USE_NONTEMPORAL_STORES_IF_ALLOWED

// Optional attributes - these are useful for a better QoI, but not required to be available

#  define MSTD_NOALIAS __attribute__((__malloc__))
#  define MSTD_NODEBUG [[__gnu__::__nodebug__]]
#  define MSTD_NO_SANITIZE(...) __attribute__((__no_sanitize__(__VA_ARGS__)))
#  define MSTD_INIT_PRIORITY_MAX __attribute__((__init_priority__(100)))
#  define MSTD_ATTRIBUTE_FORMAT(archetype, format_string_index, first_format_arg_index)                             \
    __attribute__((__format__(archetype, format_string_index, first_format_arg_index)))
#  define MSTD_PACKED __attribute__((__packed__))

#  if __has_attribute(__no_sanitize__) && !defined(MSTD_COMPILER_GCC)
#    define MSTD_NO_CFI __attribute__((__no_sanitize__("cfi")))
#  else
#    define MSTD_NO_CFI
#  endif

#  if __has_attribute(__using_if_exists__)
#    define MSTD_USING_IF_EXISTS __attribute__((__using_if_exists__))
#  else
#    define MSTD_USING_IF_EXISTS
#  endif

#  if __has_cpp_attribute(_Clang::__no_destroy__)
#    define MSTD_NO_DESTROY [[_Clang::__no_destroy__]]
#  else
#    define MSTD_NO_DESTROY
#  endif

#  if __has_attribute(__diagnose_if__)
#    define MSTD_DIAGNOSE_WARNING(...) __attribute__((__diagnose_if__(__VA_ARGS__, "warning")))
#  else
#    define MSTD_DIAGNOSE_WARNING(...)
#  endif

#  if __has_attribute(__diagnose_if__) && !defined(MSTD_APPLE_CLANG_VER) &&                                         \
      (!defined(MSTD_CLANG_VER) || MSTD_CLANG_VER >= 2001)
#    define MSTD_DIAGNOSE_IF(...) __attribute__((__diagnose_if__(__VA_ARGS__)))
#  else
#    define MSTD_DIAGNOSE_IF(...)
#  endif

#  define MSTD_DIAGNOSE_NULLPTR_IF(condition, condition_description)                                                \
    MSTD_DIAGNOSE_IF(                                                                                               \
        condition,                                                                                                     \
        "null passed to callee that requires a non-null argument" condition_description,                               \
        "warning",                                                                                                     \
        "nonnull")

#  if __has_cpp_attribute(_Clang::__lifetimebound__)
#    define MSTD_LIFETIMEBOUND [[_Clang::__lifetimebound__]]
#  else
#    define MSTD_LIFETIMEBOUND
#  endif

// This is to work around https://llvm.org/PR156809
#  define MSTD_CTOR_LIFETIMEBOUND MSTD_LIFETIMEBOUND

#  if __has_cpp_attribute(_Clang::__noescape__)
#    define MSTD_NOESCAPE [[_Clang::__noescape__]]
#  else
#    define MSTD_NOESCAPE
#  endif

#  if __has_cpp_attribute(_Clang::__no_specializations__)
#    define MSTD_NO_SPECIALIZATIONS                                                                                 \
      [[_Clang::__no_specializations__("Users are not allowed to specialize this standard library entity")]]
#  else
#    define MSTD_NO_SPECIALIZATIONS
#  endif

#  if __has_cpp_attribute(_Clang::__preferred_name__)
#    define MSTD_PREFERRED_NAME(x) [[_Clang::__preferred_name__(x)]]
#  else
#    define MSTD_PREFERRED_NAME(x)
#  endif

#  if __has_cpp_attribute(_Clang::__scoped_lockable__)
#    define MSTD_SCOPED_LOCKABLE [[_Clang::__scoped_lockable__]]
#  else
#    define MSTD_SCOPED_LOCKABLE
#  endif

#  if __has_cpp_attribute(_Clang::__capability__)
#    define MSTD_CAPABILITY(...) [[_Clang::__capability__(__VA_ARGS__)]]
#  else
#    define MSTD_CAPABILITY(...)
#  endif

#  if __has_attribute(__acquire_capability__)
#    define MSTD_ACQUIRE_CAPABILITY(...) __attribute__((__acquire_capability__(__VA_ARGS__)))
#  else
#    define MSTD_ACQUIRE_CAPABILITY(...)
#  endif

#  if __has_cpp_attribute(_Clang::__try_acquire_capability__)
#    define MSTD_TRY_ACQUIRE_CAPABILITY(...) [[_Clang::__try_acquire_capability__(__VA_ARGS__)]]
#  else
#    define MSTD_TRY_ACQUIRE_CAPABILITY(...)
#  endif

#  if __has_cpp_attribute(_Clang::__acquire_shared_capability__)
#    define MSTD_ACQUIRE_SHARED_CAPABILITY [[_Clang::__acquire_shared_capability__]]
#  else
#    define MSTD_ACQUIRE_SHARED_CAPABILITY
#  endif

#  if __has_cpp_attribute(_Clang::__try_acquire_shared_capability__)
#    define MSTD_TRY_ACQUIRE_SHARED_CAPABILITY(...) [[_Clang::__try_acquire_shared_capability__(__VA_ARGS__)]]
#  else
#    define MSTD_TRY_ACQUIRE_SHARED_CAPABILITY(...)
#  endif

#  if __has_cpp_attribute(_Clang::__release_capability__)
#    define MSTD_RELEASE_CAPABILITY [[_Clang::__release_capability__]]
#  else
#    define MSTD_RELEASE_CAPABILITY
#  endif

#  if __has_cpp_attribute(_Clang::__release_shared_capability__)
#    define MSTD_RELEASE_SHARED_CAPABILITY [[_Clang::__release_shared_capability__]]
#  else
#    define MSTD_RELEASE_SHARED_CAPABILITY
#  endif

#  if __has_attribute(__requires_capability__)
#    define MSTD_REQUIRES_CAPABILITY(...) __attribute__((__requires_capability__(__VA_ARGS__)))
#  else
#    define MSTD_REQUIRES_CAPABILITY(...)
#  endif

#  if defined(MSTD_ABI_MICROSOFT) && __has_declspec_attribute(empty_bases)
#    define MSTD_DECLSPEC_EMPTY_BASES __declspec(empty_bases)
#  else
#    define MSTD_DECLSPEC_EMPTY_BASES
#  endif

// Allow for build-time disabling of unsigned integer sanitization
#  if __has_attribute(no_sanitize) && !defined(MSTD_COMPILER_GCC)
#    define MSTD_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK __attribute__((__no_sanitize__("unsigned-integer-overflow")))
#  else
#    define MSTD_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
#  endif

#  if __has_feature(nullability)
#    define MSTD_DIAGNOSE_NULLPTR _Nonnull
#  else
#    define MSTD_DIAGNOSE_NULLPTR
#  endif

#endif // __cplusplus

#endif // MSTD___CONFIG
