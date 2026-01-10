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

#  define MSTD_CONCAT_IMPL(_X, _Y) _X##_Y
#  define MSTD_CONCAT(_X, _Y) MSTD_CONCAT_IMPL(_X, _Y)
#  define MSTD_CONCAT3(X, Y, Z) MSTD_CONCAT(X, MSTD_CONCAT(Y, Z))
#  define MSTD_TOSTRING2(x) #x
#  define MSTD_TOSTRING(x) MSTD_TOSTRING2(x)

#  if defined(MSTD_OBJECT_FORMAT_COFF)
#    if defined(MSTD_BUILDING_LIBRARY)
#      define MSTD_OVERRIDABLE_FUNC_VIS __declspec(dllexport)
#    else
#      define MSTD_OVERRIDABLE_FUNC_VIS
#    endif

#    define MSTD_NAMESPACE_VISIBILITY

#  else

#    if !defined(MSTD_DISABLE_VISIBILITY_ANNOTATIONS)
#      define MSTD_VISIBILITY(vis) __attribute__((__visibility__(vis)))
#    else
#      define MSTD_VISIBILITY(vis)
#    endif

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
        MSTD_CONCAT(MSTD_HARDENING_SIG, MSTD_ASSERTION_SEMANTIC_SIG), MSTD_EXCEPTIONS_SIG)

#  ifndef MSTD_NO_AUTO_LINK
#    if defined(MSTD_ABI_MICROSOFT) && !defined(MSTD_BUILDING_LIBRARY)
#      if !defined(MSTD_DISABLE_VISIBILITY_ANNOTATIONS)
#        pragma comment(lib, "c++.lib")
#      else
#        pragma comment(lib, "libc++.lib")
#      endif
#    endif // defined(MSTD_ABI_MICROSOFT) && !defined(MSTD_BUILDING_LIBRARY)
#  endif   // MSTD_NO_AUTO_LINK

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
