// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_NODE_HANDLE
#define MSTD_NODE_HANDLE

#include <cassert>
#include <memory>
#include <optional>

namespace mstd {

// Specialized in Tree & __hash_table for their _NodeType.
template <class NodeT, class AllocT>
struct __generic_container_node_destructor;

template <class NodeT, class AllocT>
class BasicNodeHandle {
  
  template <class /*Value*/, class /*KeyProj*/, class /*Compare*/, class /*Allocator*/>
  friend class Tree;

  using AllocTraits_ = std::allocator_traits<AllocT>;
  using NodePointerType_ = std::pointer_traits<typename AllocTraits_::void_pointer>::template rebind<NodeT>;

public:
  using allocator_type = AllocT;

private:
  NodePointerType_ ptr_ = nullptr;
  std::optional<allocator_type> alloc_;

  void releasePtr_() {
    ptr_   = nullptr;
    alloc_ = std::nullopt;
  }

  void destroyNodePointer_() {
    if (ptr_ != nullptr) {
      using NodeAllocType = std::allocator_traits<allocator_type>::template rebind_alloc<NodeT>; 
      NodeAllocType alloc(*alloc_);
      __generic_container_node_destructor<NodeT, NodeAllocType>(alloc, true)(ptr_);
      ptr_ = nullptr;
    }
  }

  BasicNodeHandle(NodePointerType_ ptr, allocator_type const& alloc)
      : ptr_(ptr)
      , alloc_(alloc) {}

public:
  BasicNodeHandle() = default;

  BasicNodeHandle(BasicNodeHandle&& other) noexcept
      : ptr_(other.ptr_)
      , alloc_(std::move(other.alloc_)) {
    other.ptr_   = nullptr;
    other.alloc_ = std::nullopt;
  }

  BasicNodeHandle& operator=(BasicNodeHandle&& other) {
    MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
        alloc_ == std::nullopt || AllocTraits_::propagate_on_container_move_assignment::value ||
            alloc_ == other.alloc_,
        "node_type with incompatible allocator passed to "
        "node_type::operator=(node_type&&)");

    destroyNodePointer_();
    ptr_ = other.ptr_;

    if (AllocTraits_::propagate_on_container_move_assignment::value || alloc_ == std::nullopt) {
      alloc_ = std::move(other.alloc_);
    }

    other.ptr_   = nullptr;
    other.alloc_ = std::nullopt;

    return *this;
  }

  allocator_type get_allocator() const { return *alloc_; }

  explicit operator bool() const { return ptr_ != nullptr; }

  [[nodiscard]] bool empty() const { return ptr_ == nullptr; }

  void swap(BasicNodeHandle& other) noexcept(
      AllocTraits_::propagate_on_container_swap::value || AllocTraits_::is_always_equal::value) {
    using std::swap;
    swap(ptr_, other.ptr_);
    if (AllocTraits_::propagate_on_container_swap::value 
        || alloc_ == std::nullopt
        || other.alloc_ == std::nullopt) {
      swap(alloc_, other.alloc_);
    }
  }

  friend void
  swap(BasicNodeHandle& lhs, BasicNodeHandle& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }

  ~BasicNodeHandle() { destroyNodePointer_(); }
};

template <class NodeT, class AllocT>
using SetNodeHandle = BasicNodeHandle< NodeT, AllocT>;

template <class NodeT, class AllocT>
using MapNodeHandle = BasicNodeHandle< NodeT, AllocT>;

template <class _Iterator, class NodeT>
struct __insert_return_type {
  _Iterator position;
  bool inserted;
  NodeT node;
};

} // namespace mstd


#endif // MSTD_NODE_HANDLE
