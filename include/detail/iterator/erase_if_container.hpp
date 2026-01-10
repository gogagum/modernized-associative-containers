// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_ITERATOR_ERASE_IF_CONTAINER_HPP
#define MSTD_ITERATOR_ERASE_IF_CONTAINER_HPP

#include <detail/config.hpp>

#if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif


namespace mstd {

template <class _Container, class _Predicate>
MSTD_HIDE_FROM_ABI typename _Container::size_type _MSTD_erase_if_container(_Container& __c, _Predicate& __pred) {
  typename _Container::size_type __old_size = __c.size();

  const typename _Container::iterator __last = __c.end();
  for (typename _Container::iterator __iter = __c.begin(); __iter != __last;) {
    if (__pred(*__iter))
      __iter = __c.erase(__iter);
    else
      ++__iter;
  }

  return __old_size - __c.size();
}

} // namespace mstd


#endif // MSTD_ITERATOR_ERASE_IF_CONTAINER_HPP
