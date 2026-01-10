//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_TYPE_TRAITS_NAT_HPP
#define MSTD_TYPE_TRAITS_NAT_HPP

#include "detail/config.hpp"

namespace mstd {

struct __nat {
  __nat()                        = delete;
  __nat(const __nat&)            = delete;
  __nat& operator=(const __nat&) = delete;
  ~__nat()                       = delete;
};

} // namespace mstd

#endif // MSTD_TYPE_TRAITS_NAT_HPP
