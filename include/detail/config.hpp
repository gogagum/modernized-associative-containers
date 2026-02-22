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

#include "configuration/compiler.hpp"

#ifdef __cplusplus

#  define MSTD_CONCAT_IMPL(_X, _Y) _X##_Y
#  define MSTD_CONCAT(_X, _Y) MSTD_CONCAT_IMPL(_X, _Y)
#  define MSTD_CONCAT3(X, Y, Z) MSTD_CONCAT(X, MSTD_CONCAT(Y, Z))

#endif // __cplusplus

#endif // MSTD___CONFIG
