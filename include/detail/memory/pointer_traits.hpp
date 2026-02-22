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

namespace mstd {

template <class FromT, class ToT>
using __rebind_pointer_t = typename std::pointer_traits<FromT>::template rebind<ToT>;

} // namespace mstd

#endif // MSTD_MEMORY_POINTER_TRAITS_HPP