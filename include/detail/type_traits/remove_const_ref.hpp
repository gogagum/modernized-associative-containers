//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_REMOVE_CONST_REF_HPP
#define MSTD_TYPE_TRAITS_REMOVE_CONST_REF_HPP

#include <detail/config.hpp>

#include <type_traits>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

template <class _Tp>
using __remove_const_ref_t = std::remove_const_t<std::remove_reference_t<_Tp> >;

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_REMOVE_CONST_REF_HPP
