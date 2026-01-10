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

// '__is_identifier' returns '0' if '__x' is a reserved identifier provided by
// the compiler and '1' otherwise.
#  ifndef __is_identifier
#    define __is_identifier(__x) 1
#  endif

#  if defined(__MVS__)
#    include <features.h> // for __NATIVE_ASCII_F
#  endif

#  if defined(MSTD_OBJECT_FORMAT_COFF)
#    if defined(MSTD_BUILDING_LIBRARY)
#      define MSTD_OVERRIDABLE_FUNC_VIS __declspec(dllexport)
#    else
#      define MSTD_OVERRIDABLE_FUNC_VIS
#    endif

#    define MSTD_HIDDEN
#    define MSTD_NAMESPACE_VISIBILITY

#  else

#    if !defined(MSTD_DISABLE_VISIBILITY_ANNOTATIONS)
#      define MSTD_VISIBILITY(vis) __attribute__((__visibility__(vis)))
#    else
#      define MSTD_VISIBILITY(vis)
#    endif

#    define MSTD_HIDDEN MSTD_VISIBILITY("hidden")

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
#    define MSTD_EXCLUDE_FROM_EXPLICIT_INSTANTIATION __attribute__((__always_inline__))
#  endif

#  ifdef MSTD_COMPILER_CLANG_BASED
#    define MSTD_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
#    define MSTD_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#    define MSTD_CLANG_DIAGNOSTIC_IGNORED(str) _Pragma(MSTD_TOSTRING(clang diagnostic ignored str))
#  elif defined(MSTD_COMPILER_GCC)
#    define MSTD_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#    define MSTD_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#    define MSTD_CLANG_DIAGNOSTIC_IGNORED(str)
#  else
#    define MSTD_DIAGNOSTIC_PUSH
#    define MSTD_DIAGNOSTIC_POP
#    define MSTD_CLANG_DIAGNOSTIC_IGNORED(str)
#  endif

// Macros to enter and leave a state where deprecation warnings are suppressed.
#  define MSTD_SUPPRESS_DEPRECATED_PUSH                                                                             \
    MSTD_DIAGNOSTIC_PUSH MSTD_CLANG_DIAGNOSTIC_IGNORED("-Wdeprecated")                                           \
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


#  if __has_cpp_attribute(msvc::no_unique_address)
// MSVC implements [[no_unique_address]] as a silent no-op currently.
// (If/when MSVC breaks its C++ ABI, it will be changed to work as intended.)
// However, MSVC implements [[msvc::no_unique_address]] which does what
// [[no_unique_address]] is supposed to do, in general.
#    define MSTD_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#  else
#    define MSTD_NO_UNIQUE_ADDRESS [[__no_unique_address__]]
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

// Optional attributes - these are useful for a better QoI, but not required to be available

#  define MSTD_NODEBUG [[__gnu__::__nodebug__]]

#  if __has_attribute(__diagnose_if__)
#    define MSTD_DIAGNOSE_WARNING(...) __attribute__((__diagnose_if__(__VA_ARGS__, "warning")))
#  else
#    define MSTD_DIAGNOSE_WARNING(...)
#  endif

#  if __has_cpp_attribute(_Clang::__lifetimebound__)
#    define MSTD_LIFETIMEBOUND [[_Clang::__lifetimebound__]]
#  else
#    define MSTD_LIFETIMEBOUND
#  endif

// This is to work around https://llvm.org/PR156809
#  define MSTD_CTOR_LIFETIMEBOUND MSTD_LIFETIMEBOUND

#endif // __cplusplus

#endif // MSTD___CONFIG
