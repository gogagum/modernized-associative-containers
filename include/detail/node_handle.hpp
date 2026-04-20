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

template <class NodeT, class AllocT>
class NodeHandle {
  
  template <class /*Value*/, class /*KeyProj*/, class /*Compare*/, class /*Allocator*/>
  friend class Tree;

  using AllocTraits_ = std::allocator_traits<AllocT>;
  using NodePointerType_ = std::pointer_traits<typename AllocTraits_::void_pointer>::template rebind<NodeT>;

public:
  using allocator_type = AllocT;

private:
  NodePointerType_ ptr_ = nullptr;
  std::optional<allocator_type> alloc_;

  void destroyNodePointer_() {
    if (ptr_ != nullptr) {
      using NodeAllocType = AllocT::template rebind_alloc<NodeT>; 
      NodeAllocType alloc(*alloc_);
      typename NodeT::template Destructor<NodeAllocType>{alloc, true}(ptr_);
      ptr_ = nullptr;
    }
  }

  NodeHandle(NodePointerType_ ptr, allocator_type const& alloc)
      : ptr_(ptr)
      , alloc_(alloc) {}

public:
  NodeHandle() = default;

  NodeHandle(NodeHandle&& other) noexcept
      : ptr_(other.ptr_)
      , alloc_(std::move(other.alloc_)) {
    other.ptr_   = nullptr;
    other.alloc_ = std::nullopt;
  }

  NodeHandle& operator=(NodeHandle&& other) {
    MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
        !alloc_.has_value() || AllocTraits_::propagate_on_container_move_assignment::value ||
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

  void swap(NodeHandle& other) noexcept(
      AllocTraits_::propagate_on_container_swap::value
   || AllocTraits_::is_always_equal::value
  ) {
    std::swap(ptr_, other.ptr_);
    if (AllocTraits_::propagate_on_container_swap::value 
        || alloc_ == std::nullopt
        || other.alloc_ == std::nullopt
    ) {
      std::swap(alloc_, other.alloc_);
    }
  }

  friend void
  swap(NodeHandle& lhs, NodeHandle& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }

  ~NodeHandle() { destroyNodePointer_(); }
};

template <class IteratorT, class NodeT>
struct NodeHandleInsertReturnType {
  IteratorT position;
  bool inserted;
  NodeT node;
};

template <class IteratorT>
struct InsertReturnType {
  IteratorT position;
  bool inserted;
};

} // namespace mstd


#endif // MSTD_NODE_HANDLE
