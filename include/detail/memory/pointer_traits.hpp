// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_MEMORY_POINTER_TRAITS_HPP
#define MSTD_MEMORY_POINTER_TRAITS_HPP

#include "detail/config.hpp"

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include "detail/type_traits/nat.hpp"
#include "detail/type_traits/detected_or.hpp"
#include "detail/type_traits/conjunction.hpp"

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

namespace mstd {

template <class _From, class _To>
using __rebind_pointer_t = typename std::pointer_traits<_From>::template rebind<_To>;

} // namespace mstd

#endif // MSTD_MEMORY_POINTER_TRAITS_HPP