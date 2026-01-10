//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_IS_PRIMARY_TEMPLATE_HPP
#define MSTD_TYPE_TRAITS_IS_PRIMARY_TEMPLATE_HPP

#include <detail/config.hpp>
#include <type_traits>
#include "is_valid_expansion.hpp"

namespace mstd {

template <class _Tp>
using __test_for_primary_template =
    std::enable_if_t<std::is_same_v<_Tp, typename _Tp::__primary_template>>;

template <class _Tp>
using __is_primary_template = _IsValidExpansion<__test_for_primary_template, _Tp>;

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_IS_PRIMARY_TEMPLATE_HPP
