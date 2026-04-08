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

namespace mstd
{

  template <class ContainerT, class PredicateT>
  typename ContainerT::size_type erase_if_container(ContainerT &container, PredicateT &predicate)
  {
    typename ContainerT::size_type old_size = container.size();

    const typename ContainerT::iterator last = container.end();
    for (typename ContainerT::iterator iter = container.begin(); iter != last;)
    {
      if (predicate(*iter))
      {
        iter = container.erase(iter);
      }
      else
      {
        ++iter;
      }
    }

    return old_size - container.size();
  }

} // namespace mstd

#endif // MSTD_ITERATOR_ERASE_IF_CONTAINER_HPP
