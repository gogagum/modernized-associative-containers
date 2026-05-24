
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD___TREE
#define MSTD___TREE

#include <algorithm>
#include <cassert>
#include <utility>
#include <iterator>
#include <memory>
#include <type_traits>
#include <new>
#include <limits>
#include <functional>

#include <detail/algorithm/specialized_algorithms.hpp>
#include <detail/config.hpp>
#include <detail/utility/try_key_extraction.hpp>
#include <detail/type_traits/is_specialization.hpp>
#include <detail/type_traits/copy_cvref.hpp>
#include <detail/type_traits/make_transparent.hpp>
#include <detail/memory/pointer_traits.hpp>
#include <detail/memory/allocator_traits.hpp>
#include <detail/memory/compressed_pair.hpp>
#include <detail/memory/swap_allocator.hpp>
#include <detail/utility/lazy_synth_three_way_comparator.hpp>


#define MSTD_ASSERT_INTERNAL(stmt, message) assert((stmt) && (message));

// GCC complains about the backslashes at the end, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=121528
// Tree is a red-black-tree implementation used for the associative containers (i.e. (multi)map/set). It stores
// - (1) a pointer to the node with the smallest (i.e. leftmost) element, namely __begin_node_
// - (2) the number of nodes in the tree, namely __size_
// - (3) a pointer to the root of the tree, namely __end_node_
//
// Storing (1) and (2) is required to allow for constant time lookups. A tree looks like this in memory:
//
//       __end_node_
//            |
//           root---
//          /      |
//         /       |
//        /        |
//       /         |
//      l1        r1--
//     /  \      /   |
//   ...  ...  ...   ...
//
// All nodes except __end_node_ have a __left_ and __right_ pointer as well as a __parent_ pointer.
// __end_node_ only contains a __left_ pointer, which points to the root of the tree.
// This layout allows for iteration through the tree without a need for special handling of the end node. See
// __tree_next_iter and __tree_prev_iter for more details.

namespace mstd {

template <class _Pointer>
class __tree_end_node;
template <class _VoidPtr>
class __tree_node_base;
template <class _Tp, class _VoidPtr>
class __tree_node;

template <class KeyT, class _Value>
struct ValueType;

/*
 *
 * _NodePtr algorithms
 *
 * The algorithms taking _NodePtr are red black tree algorithms.  Those
 * algorithms taking a parameter named __root should assume that __root
 * points to a proper red black tree (unless otherwise specified).
 *
 * Each algorithm herein assumes that __root->__parent_ points to a non-null
 * structure which has a member __left_ which points back to __root.  No other
 * member is read or written to at __root->__parent_.
 *
 * __root->__parent_ will be referred to below (in comments only) as end_node.
 * end_node->__left_ is an externally accessible lvalue for __root, and can be
 * changed by node insertion and removal (without explicit reference to end_node).
 *
 * All nodes (with the exception of end_node), even the node referred to as
 * __root, have a non-null __parent_ field.
 *
 */

// Returns:  true if __x is a left child of its parent, else false
// Precondition:  __x != nullptr.
template <class _NodePtr>
inline bool __tree_is_left_child(_NodePtr __x) noexcept {
    return __x == __x->__parent_->__left_;
}

// Determines if the subtree rooted at __x is a proper red black subtree.  If
//    __x is a proper subtree, returns the black height (null counts as 1).  If
//    __x is an improper subtree, returns 0.
template <class _NodePtr>
unsigned __tree_sub_invariant(_NodePtr __x) {
    if (__x == nullptr)
        return 1;
    // parent consistency checked by caller
    // check __x->__left_ consistency
    if (__x->__left_ != nullptr && __x->__left_->__parent_ != __x)
        return 0;
    // check __x->__right_ consistency
    if (__x->__right_ != nullptr && __x->__right_->__parent_ != __x)
        return 0;
    // check __x->__left_ != __x->__right_ unless both are nullptr
    if (__x->__left_ == __x->__right_ && __x->__left_ != nullptr)
        return 0;
    // If this is red, neither child can be red
    if (!__x->__is_black_) {
        if (__x->__left_ && !__x->__left_->__is_black_)
            return 0;
        if (__x->__right_ && !__x->__right_->__is_black_)
            return 0;
    }
    unsigned __h = mstd::__tree_sub_invariant(__x->__left_);
    if (__h == 0)
        return 0; // invalid left subtree
        if (__h != mstd::__tree_sub_invariant(__x->__right_))
            return 0;                    // invalid or different height right subtree
            return __h + __x->__is_black_; // return black height of this node
}

// Determines if the red black tree rooted at __root is a proper red black tree.
//    __root == nullptr is a proper tree.  Returns true if __root is a proper
//    red black tree, else returns false.
template <class _NodePtr>
bool __tree_invariant(_NodePtr __root) {
    if (__root == nullptr)
        return true;
    // check __x->__parent_ consistency
    if (__root->__parent_ == nullptr)
        return false;
    if (!mstd::__tree_is_left_child(__root))
        return false;
    // root must be black
    if (!__root->__is_black_)
        return false;
    // do normal node checks
    return mstd::__tree_sub_invariant(__root) != 0;
}

// Returns:  pointer to the left-most node under __x.
template <class _NodePtr>
inline _NodePtr __tree_min(_NodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__x != nullptr, "Root node shouldn't be null");
    while (__x->__left_ != nullptr)
        __x = __x->__left_;
    return __x;
}

// Returns:  pointer to the right-most node under __x.
template <class _NodePtr>
inline _NodePtr __tree_max(_NodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__x != nullptr, "Root node shouldn't be null");
    while (__x->__right_ != nullptr)
        __x = __x->__right_;
    return __x;
}

// Returns:  pointer to the next in-order node after __x.
template <class _NodePtr>
_NodePtr __tree_next(_NodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__x != nullptr, "node shouldn't be null");
    if (__x->__right_ != nullptr)
        return mstd::__tree_min(__x->__right_);
    while (!mstd::__tree_is_left_child(__x))
        __x = __x->__parent_unsafe();
    return __x->__parent_unsafe();
}

// __tree_next_iter and __tree_prev_iter implement iteration through the tree. The order is as follows:
// left sub-tree -> node -> right sub-tree. When the right-most node of a sub-tree is reached, we walk up the tree until
// we find a node where we were in the left sub-tree. We are _always_ in a left sub-tree, since the __end_node_ points
// to the actual root of the tree through a __left_ pointer. Incrementing the end() pointer is UB, so we can assume that
// never happens.
template <class _EndNodePtr, class _NodePtr>
inline _EndNodePtr __tree_next_iter(_NodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__x != nullptr, "node shouldn't be null");
    if (__x->__right_ != nullptr)
        return static_cast<_EndNodePtr>(mstd::__tree_min(__x->__right_));
    while (!mstd::__tree_is_left_child(__x))
        __x = __x->__parent_unsafe();
    return static_cast<_EndNodePtr>(__x->__parent_);
}

// Returns:  pointer to the previous in-order node before __x.
// Note: __x may be the end node.
template <class _NodePtr, class _EndNodePtr>
inline _NodePtr __tree_prev_iter(_EndNodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__x != nullptr, "node shouldn't be null");
    if (__x->__left_ != nullptr)
        return mstd::__tree_max(__x->__left_);
    _NodePtr __xx = static_cast<_NodePtr>(__x);
    while (mstd::__tree_is_left_child(__xx))
        __xx = __xx->__parent_unsafe();
    return __xx->__parent_unsafe();
}

// Returns:  pointer to a node which has no children
template <class _NodePtr>
_NodePtr __tree_leaf(_NodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__x != nullptr, "node shouldn't be null");
    while (true) {
        if (__x->__left_ != nullptr) {
            __x = __x->__left_;
            continue;
        }
        if (__x->__right_ != nullptr) {
            __x = __x->__right_;
            continue;
        }
        break;
    }
    return __x;
}

// Effects:  Makes __x->__right_ the subtree root with __x as its left child
//           while preserving in-order order.
template <class _NodePtr>
void __tree_left_rotate(_NodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__x != nullptr, "node shouldn't be null");
    MSTD_ASSERT_INTERNAL(__x->__right_ != nullptr, "node should have a right child");
    _NodePtr __y  = __x->__right_;
    __x->__right_ = __y->__left_;
    if (__x->__right_ != nullptr)
        __x->__right_->__set_parent(__x);
    __y->__parent_ = __x->__parent_;
    if (mstd::__tree_is_left_child(__x))
        __x->__parent_->__left_ = __y;
    else
        __x->__parent_unsafe()->__right_ = __y;
    __y->__left_ = __x;
    __x->__set_parent(__y);
}

// Effects:  Makes __x->__left_ the subtree root with __x as its right child
//           while preserving in-order order.
template <class _NodePtr>
void __tree_right_rotate(_NodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__x != nullptr, "node shouldn't be null");
    MSTD_ASSERT_INTERNAL(__x->__left_ != nullptr, "node should have a left child");
    _NodePtr __y = __x->__left_;
    __x->__left_ = __y->__right_;
    if (__x->__left_ != nullptr)
        __x->__left_->__set_parent(__x);
    __y->__parent_ = __x->__parent_;
    if (mstd::__tree_is_left_child(__x))
        __x->__parent_->__left_ = __y;
    else
        __x->__parent_unsafe()->__right_ = __y;
    __y->__right_ = __x;
    __x->__set_parent(__y);
}

// Effects:  Rebalances __root after attaching __x to a leaf.
// Precondition:  __x has no children.
//                __x == __root or == a direct or indirect child of __root.
//                If __x were to be unlinked from __root (setting __root to
//                  nullptr if __root == __x), __tree_invariant(__root) == true.
// Postcondition: __tree_invariant(end_node->__left_) == true.  end_node->__left_
//                may be different than the value passed in as __root.
template <class _NodePtr>
void __tree_balance_after_insert(_NodePtr __root, _NodePtr __x) noexcept {
    MSTD_ASSERT_INTERNAL(__root != nullptr, "Root of the tree shouldn't be null");
    MSTD_ASSERT_INTERNAL(__x != nullptr, "Can't attach null node to a leaf");
    __x->__is_black_ = __x == __root;
    while (__x != __root && !__x->__parent_unsafe()->__is_black_) {
        // __x->__parent_ != __root because __x->__parent_->__is_black == false
        if (mstd::__tree_is_left_child(__x->__parent_unsafe())) {
            _NodePtr __y = __x->__parent_unsafe()->__parent_unsafe()->__right_;
            if (__y != nullptr && !__y->__is_black_) {
                __x              = __x->__parent_unsafe();
                __x->__is_black_ = true;
                __x              = __x->__parent_unsafe();
                __x->__is_black_ = __x == __root;
                __y->__is_black_ = true;
            } else {
                if (!mstd::__tree_is_left_child(__x)) {
                    __x = __x->__parent_unsafe();
                    mstd::__tree_left_rotate(__x);
                }
                __x              = __x->__parent_unsafe();
                __x->__is_black_ = true;
                __x              = __x->__parent_unsafe();
                __x->__is_black_ = false;
                mstd::__tree_right_rotate(__x);
                break;
            }
        } else {
            _NodePtr __y = __x->__parent_unsafe()->__parent_->__left_;
            if (__y != nullptr && !__y->__is_black_) {
                __x              = __x->__parent_unsafe();
                __x->__is_black_ = true;
                __x              = __x->__parent_unsafe();
                __x->__is_black_ = __x == __root;
                __y->__is_black_ = true;
            } else {
                if (mstd::__tree_is_left_child(__x)) {
                    __x = __x->__parent_unsafe();
                    mstd::__tree_right_rotate(__x);
                }
                __x              = __x->__parent_unsafe();
                __x->__is_black_ = true;
                __x              = __x->__parent_unsafe();
                __x->__is_black_ = false;
                mstd::__tree_left_rotate(__x);
                break;
            }
        }
    }
}

// Precondition:  __z == __root or == a direct or indirect child of __root.
// Effects:  unlinks __z from the tree rooted at __root, rebalancing as needed.
// Postcondition: __tree_invariant(end_node->__left_) == true && end_node->__left_
//                nor any of its children refer to __z.  end_node->__left_
//                may be different than the value passed in as __root.
template <class _NodePtr>
void __tree_remove(_NodePtr __root, _NodePtr __z) noexcept {
    MSTD_ASSERT_INTERNAL(__root != nullptr, "Root node should not be null");
    MSTD_ASSERT_INTERNAL(__z != nullptr, "The node to remove should not be null");
    MSTD_ASSERT_INTERNAL(mstd::__tree_invariant(__root), "The tree invariants should hold");
    // __z will be removed from the tree.  Client still needs to destruct/deallocate it
    // __y is either __z, or if __z has two children, __tree_next(__z).
    // __y will have at most one child.
    // __y will be the initial hole in the tree (make the hole at a leaf)
    _NodePtr __y = (__z->__left_ == nullptr || __z->__right_ == nullptr) ? __z : mstd::__tree_next(__z);
    // __x is __y's possibly null single child
    _NodePtr __x = __y->__left_ != nullptr ? __y->__left_ : __y->__right_;
    // __w is __x's possibly null uncle (will become __x's sibling)
    _NodePtr __w = nullptr;
    // link __x to __y's parent, and find __w
    if (__x != nullptr)
        __x->__parent_ = __y->__parent_;
    if (mstd::__tree_is_left_child(__y)) {
        __y->__parent_->__left_ = __x;
        if (__y != __root)
            __w = __y->__parent_unsafe()->__right_;
        else
            __root = __x; // __w == nullptr
    } else {
        __y->__parent_unsafe()->__right_ = __x;
        // __y can't be root if it is a right child
        __w = __y->__parent_->__left_;
    }
    bool __removed_black = __y->__is_black_;
    // If we didn't remove __z, do so now by splicing in __y for __z,
    //    but copy __z's color.  This does not impact __x or __w.
    if (__y != __z) {
        // __z->__left_ != nullptr but __z->__right_ might == __x == nullptr
        __y->__parent_ = __z->__parent_;
        if (mstd::__tree_is_left_child(__z))
            __y->__parent_->__left_ = __y;
        else
            __y->__parent_unsafe()->__right_ = __y;
        __y->__left_ = __z->__left_;
        __y->__left_->__set_parent(__y);
        __y->__right_ = __z->__right_;
        if (__y->__right_ != nullptr)
            __y->__right_->__set_parent(__y);
        __y->__is_black_ = __z->__is_black_;
        if (__root == __z)
            __root = __y;
    }
    // There is no need to rebalance if we removed a red, or if we removed
    //     the last node.
    if (__removed_black && __root != nullptr) {
        // Rebalance:
        // __x has an implicit black color (transferred from the removed __y)
        //    associated with it, no matter what its color is.
        // If __x is __root (in which case it can't be null), it is supposed
        //    to be black anyway, and if it is doubly black, then the double
        //    can just be ignored.
        // If __x is red (in which case it can't be null), then it can absorb
        //    the implicit black just by setting its color to black.
        // Since __y was black and only had one child (which __x points to), __x
        //   is either red with no children, else null, otherwise __y would have
        //   different black heights under left and right pointers.
        // if (__x == __root || __x != nullptr && !__x->__is_black_)
        if (__x != nullptr)
            __x->__is_black_ = true;
        else {
            //  Else __x isn't root, and is "doubly black", even though it may
            //     be null.  __w can not be null here, else the parent would
            //     see a black height >= 2 on the __x side and a black height
            //     of 1 on the __w side (__w must be a non-null black or a red
            //     with a non-null black child).
            while (true) {
                if (!mstd::__tree_is_left_child(__w)) // if x is left child
                {
                    if (!__w->__is_black_) {
                        __w->__is_black_                    = true;
                        __w->__parent_unsafe()->__is_black_ = false;
                        mstd::__tree_left_rotate(__w->__parent_unsafe());
                        // __x is still valid
                        // reset __root only if necessary
                        if (__root == __w->__left_)
                            __root = __w;
                        // reset sibling, and it still can't be null
                        __w = __w->__left_->__right_;
                    }
                    // __w->__is_black_ is now true, __w may have null children
                    if ((__w->__left_ == nullptr || __w->__left_->__is_black_) &&
                        (__w->__right_ == nullptr || __w->__right_->__is_black_)) {
                        __w->__is_black_ = false;
                    __x              = __w->__parent_unsafe();
                    // __x can no longer be null
                    if (__x == __root || !__x->__is_black_) {
                        __x->__is_black_ = true;
                        break;
                    }
                    // reset sibling, and it still can't be null
                    __w = mstd::__tree_is_left_child(__x) ? __x->__parent_unsafe()->__right_ : __x->__parent_->__left_;
                    // continue;
                        } else // __w has a red child
                        {
                            if (__w->__right_ == nullptr || __w->__right_->__is_black_) {
                                // __w left child is non-null and red
                                __w->__left_->__is_black_ = true;
                                __w->__is_black_          = false;
                                mstd::__tree_right_rotate(__w);
                                // __w is known not to be root, so root hasn't changed
                                // reset sibling, and it still can't be null
                                __w = __w->__parent_unsafe();
                            }
                            // __w has a right red child, left child may be null
                            __w->__is_black_                    = __w->__parent_unsafe()->__is_black_;
                            __w->__parent_unsafe()->__is_black_ = true;
                            __w->__right_->__is_black_          = true;
                            mstd::__tree_left_rotate(__w->__parent_unsafe());
                            break;
                        }
                } else {
                    if (!__w->__is_black_) {
                        __w->__is_black_                    = true;
                        __w->__parent_unsafe()->__is_black_ = false;
                        mstd::__tree_right_rotate(__w->__parent_unsafe());
                        // __x is still valid
                        // reset __root only if necessary
                        if (__root == __w->__right_)
                            __root = __w;
                        // reset sibling, and it still can't be null
                        __w = __w->__right_->__left_;
                    }
                    // __w->__is_black_ is now true, __w may have null children
                    if ((__w->__left_ == nullptr || __w->__left_->__is_black_) &&
                        (__w->__right_ == nullptr || __w->__right_->__is_black_)) {
                        __w->__is_black_ = false;
                    __x              = __w->__parent_unsafe();
                    // __x can no longer be null
                    if (!__x->__is_black_ || __x == __root) {
                        __x->__is_black_ = true;
                        break;
                    }
                    // reset sibling, and it still can't be null
                    __w = mstd::__tree_is_left_child(__x) ? __x->__parent_unsafe()->__right_ : __x->__parent_->__left_;
                    // continue;
                        } else // __w has a red child
                        {
                            if (__w->__left_ == nullptr || __w->__left_->__is_black_) {
                                // __w right child is non-null and red
                                __w->__right_->__is_black_ = true;
                                __w->__is_black_           = false;
                                mstd::__tree_left_rotate(__w);
                                // __w is known not to be root, so root hasn't changed
                                // reset sibling, and it still can't be null
                                __w = __w->__parent_unsafe();
                            }
                            // __w has a left red child, right child may be null
                            __w->__is_black_                    = __w->__parent_unsafe()->__is_black_;
                            __w->__parent_unsafe()->__is_black_ = true;
                            __w->__left_->__is_black_           = true;
                            mstd::__tree_right_rotate(__w->__parent_unsafe());
                            break;
                        }
                }
            }
        }
    }
}

// node traits

template <class _Tp>
inline const bool __is_tree_value_type_v = __is_specialization_v<_Tp, ValueType>;

template <class _Tp>
struct __get_tree_key_type {
    using type = _Tp;
};

template <class KeyT, class _ValueT>
struct __get_tree_key_type<ValueType<KeyT, _ValueT> > {
    using type = KeyT;
};

template <class _Tp>
using __get_tree_key_type_t = typename __get_tree_key_type<_Tp>::type;

template <class _Tp>
struct __get_node_value_type {
    using type = _Tp;
};

template <class KeyT, class _ValueT>
struct __get_node_value_type<ValueType<KeyT, _ValueT> > {
    using type = std::pair<const KeyT, _ValueT>;
};

template <class _Tp>
using __get_node_value_type_t = typename __get_node_value_type<_Tp>::type;

template <class _NodePtr, class _NodeT = typename std::pointer_traits<_NodePtr>::element_type>
struct __tree_node_types;

template <class _NodePtr, class _Tp, class _VoidPtr>
struct __tree_node_types<_NodePtr, __tree_node<_Tp, _VoidPtr> > {
    using __node_base_pointer = __rebind_pointer_t<_VoidPtr, __tree_node_base<_VoidPtr> >;
    using __end_node_pointer  = __rebind_pointer_t<_VoidPtr, __tree_end_node<__node_base_pointer> >;

private:
    static_assert(std::is_same_v<typename std::pointer_traits<_VoidPtr>::element_type, void>,
                  "_VoidPtr does not point to unqualified void type");
};

// node

template <class _Pointer>
class __tree_end_node {
public:
    using pointer = _Pointer;
    pointer __left_;

    __tree_end_node() noexcept : __left_() {}
};

template <class _VoidPtr>
class __tree_node_base : public __tree_end_node<__rebind_pointer_t<_VoidPtr, __tree_node_base<_VoidPtr> > > {
public:
    using pointer                            = __rebind_pointer_t<_VoidPtr, __tree_node_base>;
    using __end_node_pointer = __rebind_pointer_t<_VoidPtr, __tree_end_node<pointer> >;

    pointer __right_;
    __end_node_pointer __parent_;
    bool __is_black_;

    pointer __parent_unsafe() const { return static_cast<pointer>(__parent_); }

    void __set_parent(pointer __p) { __parent_ = static_cast<__end_node_pointer>(__p); }

    __tree_node_base()             = default;
    __tree_node_base(__tree_node_base const&)            = delete;
    __tree_node_base& operator=(__tree_node_base const&) = delete;
};

template <class _Tp, class _VoidPtr>
class __tree_node : public __tree_node_base<_VoidPtr> {
public:
    using __node_value_type = __get_node_value_type_t<_Tp>;

    // We use a union to avoid initialization during member initialization, which allows us
    // to use the allocator from the container to construct the `__node_value_type` in the
    // memory provided by the union member

private:
    union {
        __node_value_type __value_;
    };

public:
    __node_value_type& __get_value() { return __value_; }

    template <class _Alloc, class... _Args>
    explicit __tree_node(_Alloc& __na, _Args&&... __args) {
        std::allocator_traits<_Alloc>::construct(__na, std::addressof(__get_value()), std::forward<_Args>(__args)...);
    }
    ~__tree_node()                             = delete;
    __tree_node(__tree_node const&)            = delete;
    __tree_node& operator=(__tree_node const&) = delete;
};

template <class _Allocator>
class __tree_node_destructor {
    using allocator_type                 = _Allocator;
    using __alloc_traits = std::allocator_traits<allocator_type>;

public:
    using pointer = typename __alloc_traits::pointer;

private:
    allocator_type& na_;

public:
    bool __value_constructed;

    __tree_node_destructor(const __tree_node_destructor&) = default;
    __tree_node_destructor& operator=(const __tree_node_destructor&)            = delete;

    explicit __tree_node_destructor(allocator_type& __na, bool __val = false) noexcept
    : na_(__na),
    __value_constructed(__val) {}

    void operator()(pointer __p) noexcept {
        if (__value_constructed)
            __alloc_traits::destroy(na_, std::addressof(__p->__get_value()));
        if (__p)
            __alloc_traits::deallocate(na_, __p, 1);
    }

    template <class>
    friend class MapNodeDestructor;
};

template <class _NodeType, class _Alloc>
struct __generic_container_node_destructor;
template <class _Tp, class _VoidPtr, class _Alloc>
struct __generic_container_node_destructor<__tree_node<_Tp, _VoidPtr>, _Alloc> : __tree_node_destructor<_Alloc> {
    using __tree_node_destructor<_Alloc>::__tree_node_destructor;
};

// Do an in-order traversal of the tree until `__break` returns true. Takes the root node of the tree.
template <class _Reference, class _Break, class _NodePtr, class _Func, class _Proj>
bool __tree_iterate_from_root(_Break __break, _NodePtr __root, _Func& __func, _Proj& __proj) {
    if (__root->__left_) {
        if (mstd::__tree_iterate_from_root<_Reference>(__break, static_cast<_NodePtr>(__root->__left_), __func, __proj))
            return true;
    }
    if (__break(__root))
        return true;
    std::invoke(__func, std::invoke(__proj, static_cast<_Reference>(__root->__get_value())));
    if (__root->__right_)
        return mstd::__tree_iterate_from_root<_Reference>(__break, static_cast<_NodePtr>(__root->__right_), __func, __proj);
    return false;
}

// Do an in-order traversal of the tree from __first to __last.
template <class _NodeIter, class _Func, class _Proj>
void
__tree_iterate_subrange(_NodeIter __first_it, _NodeIter __last_it, _Func& __func, _Proj& __proj) {
    using _NodePtr   = typename _NodeIter::__node_pointer;
    using _Reference = typename _NodeIter::reference;

    auto __first = __first_it.__ptr_;
    auto __last  = __last_it.__ptr_;

    while (true) {
        if (__first == __last)
            return;
        const auto __nfirst = static_cast<_NodePtr>(__first);
        std::invoke(__func, std::invoke(__proj, static_cast<_Reference>(__nfirst->__get_value())));
        if (__nfirst->__right_) {
            if (mstd::__tree_iterate_from_root<_Reference>(
                [&](_NodePtr __node) -> bool { return __node == __last; },
                                                          static_cast<_NodePtr>(__nfirst->__right_),
                                                          __func,
                                                          __proj))
                return;
        }
        while (!mstd::__tree_is_left_child(static_cast<_NodePtr>(__first)))
            __first = static_cast<_NodePtr>(__first)->__parent_;
        __first = static_cast<_NodePtr>(__first)->__parent_;
    }
}

template <class _Tp, class _NodePtr, class _DiffType>
class __tree_iterator {
    using _NodeTypes = __tree_node_types<_NodePtr>;
    // NOLINTNEXTLINE(libcpp-nodebug-on-aliases) lldb relies on this alias for pretty printing
    using __node_pointer                      = _NodePtr;
    using __node_base_pointer = typename _NodeTypes::__node_base_pointer;
    using __end_node_pointer  = typename _NodeTypes::__end_node_pointer;

    __end_node_pointer __ptr_;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = __get_node_value_type_t<_Tp>;
    using difference_type   = _DiffType;
    using reference         = value_type&;
    using pointer           = __rebind_pointer_t<_NodePtr, value_type>;

    __tree_iterator() noexcept : __ptr_(nullptr) {}

    reference operator*() const { return __get_np()->__get_value(); }
    pointer operator->() const {
        return std::pointer_traits<pointer>::pointer_to(__get_np()->__get_value());
    }

    __tree_iterator& operator++() {
        __ptr_ = mstd::__tree_next_iter<__end_node_pointer>(static_cast<__node_base_pointer>(__ptr_));
        return *this;
    }
    __tree_iterator operator++(int) {
        __tree_iterator __t(*this);
        ++(*this);
        return __t;
    }

    __tree_iterator& operator--() {
        __ptr_ = static_cast<__end_node_pointer>(mstd::__tree_prev_iter<__node_base_pointer>(__ptr_));
        return *this;
    }
    __tree_iterator operator--(int) {
        __tree_iterator __t(*this);
        --(*this);
        return __t;
    }

    friend bool operator==(const __tree_iterator& __x, const __tree_iterator& __y) {
        return __x.__ptr_ == __y.__ptr_;
    }
    friend bool operator!=(const __tree_iterator& __x, const __tree_iterator& __y) {
        return !(__x == __y);
    }

private:
    explicit __tree_iterator(__node_pointer __p) noexcept : __ptr_(__p) {}
    explicit __tree_iterator(__end_node_pointer __p) noexcept : __ptr_(__p) {}
    __node_pointer __get_np() const { return static_cast<__node_pointer>(__ptr_); }
    template <class, class, class>
    friend class Tree;
    template <class, class, class>
    friend class __tree_const_iterator;

    template <class _NodeIter, class _Func, class _Proj>
    friend void __tree_iterate_subrange(_NodeIter, _NodeIter, _Func&, _Proj&);
};

// This also handles {multi,}set::iterator, since they're just aliases to Tree::iterator
template <class _Tp, class _NodePtr, class _DiffType>
struct __specialized_algorithm<
_Algorithm::__for_each,
__iterator_pair<__tree_iterator<_Tp, _NodePtr, _DiffType>, __tree_iterator<_Tp, _NodePtr, _DiffType>>> {
    static const bool __has_algorithm = true;

    using __iterator = __tree_iterator<_Tp, _NodePtr, _DiffType>;

    template <class _Func, class _Proj>
    static void operator()(__iterator __first, __iterator __last, _Func& __func, _Proj& __proj) {
        mstd::__tree_iterate_subrange(__first, __last, __func, __proj);
    }
};

template <class _Tp, class _NodePtr, class _DiffType>
class __tree_const_iterator {
    using _NodeTypes = __tree_node_types<_NodePtr>;
    // NOLINTNEXTLINE(libcpp-nodebug-on-aliases) lldb relies on this alias for pretty printing
    using __node_pointer                      = _NodePtr;
    using __node_base_pointer = typename _NodeTypes::__node_base_pointer;
    using __end_node_pointer  = typename _NodeTypes::__end_node_pointer;

    __end_node_pointer __ptr_;

public:
    using iterator_category                    = std::bidirectional_iterator_tag;
    using value_type                           = __get_node_value_type_t<_Tp>;
    using difference_type                      = _DiffType;
    using reference                            = const value_type&;
    using pointer                              = __rebind_pointer_t<_NodePtr, const value_type>;
    using __non_const_iterator = __tree_iterator<_Tp, __node_pointer, difference_type>;

    __tree_const_iterator() noexcept : __ptr_(nullptr) {}

    __tree_const_iterator(__non_const_iterator __p) noexcept : __ptr_(__p.__ptr_) {}

    reference operator*() const { return __get_np()->__get_value(); }
    pointer operator->() const {
        return std::pointer_traits<pointer>::pointer_to(__get_np()->__get_value());
    }

    __tree_const_iterator& operator++() {
        __ptr_ = mstd::__tree_next_iter<__end_node_pointer>(static_cast<__node_base_pointer>(__ptr_));
        return *this;
    }

    __tree_const_iterator operator++(int) {
        __tree_const_iterator __t(*this);
        ++(*this);
        return __t;
    }

    __tree_const_iterator& operator--() {
        __ptr_ = static_cast<__end_node_pointer>(mstd::__tree_prev_iter<__node_base_pointer>(__ptr_));
        return *this;
    }

    __tree_const_iterator operator--(int) {
        __tree_const_iterator __t(*this);
        --(*this);
        return __t;
    }

    friend bool operator==(const __tree_const_iterator& __x, const __tree_const_iterator& __y) {
        return __x.__ptr_ == __y.__ptr_;
    }
    friend bool operator!=(const __tree_const_iterator& __x, const __tree_const_iterator& __y) {
        return !(__x == __y);
    }

private:
    explicit __tree_const_iterator(__node_pointer __p) noexcept : __ptr_(__p) {}
    explicit __tree_const_iterator(__end_node_pointer __p) noexcept : __ptr_(__p) {}
    __node_pointer __get_np() const { return static_cast<__node_pointer>(__ptr_); }

    template <class, class, class>
    friend class Tree;

    template <class _NodeIter, class _Func, class _Proj>
    friend void __tree_iterate_subrange(_NodeIter, _NodeIter, _Func&, _Proj&);
};

// This also handles {multi,}set::const_iterator, since they're just aliases to Tree::iterator
template <class _Tp, class _NodePtr, class _DiffType>
struct __specialized_algorithm<
_Algorithm::__for_each,
__iterator_pair<__tree_const_iterator<_Tp, _NodePtr, _DiffType>, __tree_const_iterator<_Tp, _NodePtr, _DiffType>>> {
    static const bool __has_algorithm = true;

    using __iterator = __tree_const_iterator<_Tp, _NodePtr, _DiffType>;

    template <class _Func, class _Proj>
    static void operator()(__iterator __first, __iterator __last, _Func& __func, _Proj& __proj) {
        mstd::__tree_iterate_subrange(__first, __last, __func, __proj);
    }
};

template <class _Tp, class _Compare>
int __diagnose_non_const_comparator();

template <class _Tp, class _Compare, class _Allocator>
class Tree {
public:
    using value_type     = __get_node_value_type_t<_Tp>;
    using value_compare  = _Compare;
    using allocator_type = _Allocator;

private:
    using __alloc_traits = std::allocator_traits<allocator_type>;
    using key_type                       = __get_tree_key_type_t<_Tp>;

public:
    using pointer         = typename __alloc_traits::pointer;
    using const_pointer   = typename __alloc_traits::const_pointer;
    using size_type       = typename __alloc_traits::size_type;
    using difference_type = typename __alloc_traits::difference_type;

    using __void_pointer = typename __alloc_traits::void_pointer;

    using __node = __tree_node<_Tp, __void_pointer>;
    // NOLINTNEXTLINE(libcpp-nodebug-on-aliases) lldb relies on this alias for pretty printing
    using __node_pointer = __rebind_pointer_t<__void_pointer, __node>;

    using __node_base         = __tree_node_base<__void_pointer>;
    using __node_base_pointer = __rebind_pointer_t<__void_pointer, __node_base>;

    using __end_node_t       = __tree_end_node<__node_base_pointer>;
    using __end_node_pointer = __rebind_pointer_t<__void_pointer, __end_node_t>;

    using __node_allocator = __rebind_alloc<__alloc_traits, __node>;
    using __node_traits    = std::allocator_traits<__node_allocator>;

private:
    // check for sane allocator pointer rebinding semantics. Rebinding the
    // allocator for a new pointer type should be exactly the same as rebinding
    // the pointer using 'pointer_traits'.
    static_assert(std::is_same_v<__node_pointer, typename __node_traits::pointer>,
                  "Allocator does not rebind pointers in a sane manner.");
    using __node_base_allocator = __rebind_alloc<__node_traits, __node_base>;
    using __node_base_traits    = std::allocator_traits<__node_base_allocator>;
    static_assert(std::is_same_v<__node_base_pointer, typename __node_base_traits::pointer>,
                  "Allocator does not rebind pointers in a sane manner.");

private:
    __end_node_pointer __begin_node_;
    MSTD_COMPRESSED_PAIR(__end_node_t, __end_node_, __node_allocator, __node_alloc_);
    MSTD_COMPRESSED_PAIR(size_type, __size_, value_compare, __value_comp_);

public:
    __end_node_pointer __end_node() noexcept {
        return std::pointer_traits<__end_node_pointer>::pointer_to(__end_node_);
    }
    __end_node_pointer __end_node() const noexcept {
        return std::pointer_traits<__end_node_pointer>::pointer_to(const_cast<__end_node_t&>(__end_node_));
    }
    __node_allocator& __node_alloc() noexcept { return __node_alloc_; }

private:
    const __node_allocator& __node_alloc() const noexcept { return __node_alloc_; }

public:
    allocator_type __alloc() const noexcept { return allocator_type(__node_alloc()); }

    size_type size() const noexcept { return __size_; }
    value_compare& value_comp() noexcept { return __value_comp_; }
    const value_compare& value_comp() const noexcept { return __value_comp_; }

public:
    __node_pointer __root() const noexcept {
        return static_cast<__node_pointer>(__end_node()->__left_);
    }

    __node_base_pointer* __root_ptr() const noexcept {
        return std::addressof(__end_node()->__left_);
    }

    using iterator       = __tree_iterator<_Tp, __node_pointer, difference_type>;
    using const_iterator = __tree_const_iterator<_Tp, __node_pointer, difference_type>;

    explicit Tree(const value_compare& __comp) noexcept(
        std::is_nothrow_default_constructible<__node_allocator>::value&& std::is_nothrow_copy_constructible<value_compare>::value)
    : __size_(0), __value_comp_(__comp) {
        __begin_node_ = __end_node();
    }

    explicit Tree(const allocator_type& __a)
    : __begin_node_(), __node_alloc_(__node_allocator(__a)), __size_(0) {
        __begin_node_ = __end_node();
    }

    Tree(const value_compare& __comp, const allocator_type& __a)
    : __begin_node_(), __node_alloc_(__node_allocator(__a)), __size_(0), __value_comp_(__comp) {
        __begin_node_ = __end_node();
    }

    Tree(const Tree& __t);

    Tree(const Tree& __other, const allocator_type& __alloc)
    : __begin_node_(__end_node()), __node_alloc_(__alloc), __size_(0), __value_comp_(__other.value_comp()) {
        if (__other.size() == 0)
            return;

        *__root_ptr()       = static_cast<__node_base_pointer>(__copy_construct_tree(__other.__root()));
        __root()->__parent_ = __end_node();
        __begin_node_       = static_cast<__end_node_pointer>(mstd::__tree_min(__end_node()->__left_));
        __size_             = __other.size();
    }

    Tree& operator=(const Tree& __t);
    template <class _ForwardIterator>
    void __assign_unique(_ForwardIterator __first, _ForwardIterator __last);
    Tree(Tree&& __t) noexcept(
        std::is_nothrow_move_constructible<__node_allocator>::value && std::is_nothrow_move_constructible<value_compare>::value);
    Tree(Tree&& __t, const allocator_type& __a);

    Tree& operator=(Tree&& __t)
    noexcept(std::is_nothrow_move_assignable<value_compare>::value &&
    ((__node_traits::propagate_on_container_move_assignment::value &&
    std::is_nothrow_move_assignable<__node_allocator>::value) ||
    std::allocator_traits<__node_allocator>::is_always_equal::value)) {
        __move_assign(__t, std::integral_constant<bool, __node_traits::propagate_on_container_move_assignment::value>());
        return *this;
    }

    ~Tree() {
        static_assert(std::is_copy_constructible<value_compare>::value, "Comparator must be copy-constructible.");
        destroy(__root());
    }

    iterator begin() noexcept { return iterator(__begin_node_); }
    const_iterator begin() const noexcept { return const_iterator(__begin_node_); }
    iterator end() noexcept { return iterator(__end_node()); }
    const_iterator end() const noexcept { return const_iterator(__end_node()); }

    size_type max_size() const noexcept {
        return std::min<size_type>(__node_traits::max_size(__node_alloc()), std::numeric_limits<difference_type >::max());
    }

    void clear() noexcept;

    void swap(Tree& __t)
    noexcept(std::is_nothrow_swappable_v<value_compare>);

    template <class... _Args>
    iterator __emplace_multi(_Args&&... __args);

    template <class... _Args>
    iterator __emplace_hint_multi(const_iterator __p, _Args&&... __args);

    template <class... _Args>
    std::pair<iterator, bool> __emplace_unique(_Args&&... __args) {
        return mstd::__try_key_extraction<key_type>(
            [this](const key_type& __key, _Args&&... __args2) {
                auto [__parent, __child] = __find_equal(__key);
                __node_pointer __r       = static_cast<__node_pointer>(__child);
                bool __inserted          = false;
                if (__child == nullptr) {
                    __node_holder __h = __construct_node(std::forward<_Args>(__args2)...);
                    __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__h.get()));
                    __r        = __h.release();
                    __inserted = true;
                }
                return std::pair<iterator, bool>(iterator(__r), __inserted);
            },
            [this](_Args&&... __args2) {
                __node_holder __h        = __construct_node(std::forward<_Args>(__args2)...);
                auto [__parent, __child] = __find_equal(__h->__get_value());
                __node_pointer __r       = static_cast<__node_pointer>(__child);
                bool __inserted          = false;
                if (__child == nullptr) {
                    __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__h.get()));
                    __r        = __h.release();
                    __inserted = true;
                }
                return std::pair<iterator, bool>(iterator(__r), __inserted);
            },
            std::forward<_Args>(__args)...);
    }

    template <class... _Args>
    std::pair<iterator, bool> __emplace_hint_unique(const_iterator __p, _Args&&... __args) {
        return mstd::__try_key_extraction<key_type>(
            [this, __p](const key_type& __key, _Args&&... __args2) {
                __node_base_pointer __dummy;
                auto [__parent, __child] = __find_equal(__p, __dummy, __key);
                __node_pointer __r       = static_cast<__node_pointer>(__child);
                bool __inserted          = false;
                if (__child == nullptr) {
                    __node_holder __h = __construct_node(std::forward<_Args>(__args2)...);
                    __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__h.get()));
                    __r        = __h.release();
                    __inserted = true;
                }
                return std::pair<iterator, bool>(iterator(__r), __inserted);
            },
            [this, __p](_Args&&... __args2) {
                __node_holder __h = __construct_node(std::forward<_Args>(__args2)...);
                __node_base_pointer __dummy;
                auto [__parent, __child] = __find_equal(__p, __dummy, __h->__get_value());
                __node_pointer __r       = static_cast<__node_pointer>(__child);
                if (__child == nullptr) {
                    __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__h.get()));
                    __r = __h.release();
                }
                return std::pair<iterator, bool>(iterator(__r), __child == nullptr);
            },
            std::forward<_Args>(__args)...);
    }

    template <class _InIter, class _Sent>
    void __insert_range_multi(_InIter __first, _Sent __last) {
        if (__first == __last)
            return;

        if (__root() == nullptr) { // Make sure we always have a root node
            __insert_node_at(
                __end_node(), __end_node()->__left_, static_cast<__node_base_pointer>(__construct_node(*__first).release()));
            ++__first;
        }

        auto __max_node = static_cast<__node_pointer>(mstd::__tree_max(static_cast<__node_base_pointer>(__root())));

        for (; __first != __last; ++__first) {
            __node_holder __nd = __construct_node(*__first);
            // Always check the max node first. This optimizes for sorted ranges inserted at the end.
            if (!value_comp()(__nd->__get_value(), __max_node->__get_value())) { // __node >= __max_val
                __insert_node_at(static_cast<__end_node_pointer>(__max_node),
                                 __max_node->__right_,
                                 static_cast<__node_base_pointer>(__nd.get()));
                __max_node = __nd.release();
            } else {
                __end_node_pointer __parent;
                __node_base_pointer& __child = __find_leaf_high(__parent, __nd->__get_value());
                __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__nd.release()));
            }
        }
    }

    template <class _InIter, class _Sent>
    void __insert_range_unique(_InIter __first, _Sent __last) {
        if (__first == __last)
            return;

        if (__root() == nullptr) {
            __insert_node_at(
                __end_node(), __end_node()->__left_, static_cast<__node_base_pointer>(__construct_node(*__first).release()));
            ++__first;
        }

        auto __max_node = static_cast<__node_pointer>(mstd::__tree_max(static_cast<__node_base_pointer>(__root())));

        using __reference = decltype(*__first);

        for (; __first != __last; ++__first) {
            mstd::__try_key_extraction<key_type>(
                [this, &__max_node](const key_type& __key, __reference&& __val) {
                    if (value_comp()(__max_node->__get_value(), __key)) { // __key > __max_node
                        __node_holder __nd = __construct_node(std::forward<__reference>(__val));
                        __insert_node_at(static_cast<__end_node_pointer>(__max_node),
                                         __max_node->__right_,
                                         static_cast<__node_base_pointer>(__nd.get()));
                        __max_node = __nd.release();
                    } else {
                        auto [__parent, __child] = __find_equal(__key);
                        if (__child == nullptr) {
                            __node_holder __nd = __construct_node(std::forward<__reference>(__val));
                            __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__nd.release()));
                        }
                    }
                },
                [this, &__max_node](__reference&& __val) {
                    __node_holder __nd = __construct_node(std::forward<__reference>(__val));
                    if (value_comp()(__max_node->__get_value(), __nd->__get_value())) { // __node > __max_node
                        __insert_node_at(static_cast<__end_node_pointer>(__max_node),
                                         __max_node->__right_,
                                         static_cast<__node_base_pointer>(__nd.get()));
                        __max_node = __nd.release();
                    } else {
                        auto [__parent, __child] = __find_equal(__nd->__get_value());
                        if (__child == nullptr) {
                            __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__nd.release()));
                        }
                    }
                },
                *__first);
        }
    }

    iterator __remove_node_pointer(__node_pointer) noexcept;

    template <class _NodeHandle, class _InsertReturnType>
    _InsertReturnType __node_handle_insert_unique(_NodeHandle&&);
    template <class _NodeHandle>
    iterator __node_handle_insert_unique(const_iterator, _NodeHandle&&);
    template <class _Comp2>
    void __node_handle_merge_unique(Tree<_Tp, _Comp2, _Allocator>& __source);

    template <class _NodeHandle>
    iterator __node_handle_insert_multi(_NodeHandle&&);
    template <class _NodeHandle>
    iterator __node_handle_insert_multi(const_iterator, _NodeHandle&&);
    template <class _Comp2>
    void __node_handle_merge_multi(Tree<_Tp, _Comp2, _Allocator>& __source);

    template <class _NodeHandle>
    _NodeHandle __node_handle_extract(key_type const&);
    template <class _NodeHandle>
    _NodeHandle __node_handle_extract(const_iterator);

    iterator erase(const_iterator __p);
    iterator erase(const_iterator __f, const_iterator __l);
    template <class KeyT>
    size_type __erase_unique(const KeyT& __k);
    template <class KeyT>
    size_type __erase_multi(const KeyT& __k);

    void
    __insert_node_at(__end_node_pointer __parent, __node_base_pointer& __child, __node_base_pointer __new_node) noexcept;

    template <class KeyT>
    iterator find(const KeyT& __key) {
        auto [__, __match] = __find_equal(__key);
        if (__match == nullptr)
            return end();
        return iterator(static_cast<__node_pointer>(__match));
    }

    template <class KeyT>
    const_iterator find(const KeyT& __key) const {
        auto [__, __match] = __find_equal(__key);
        if (__match == nullptr)
            return end();
        return const_iterator(static_cast<__node_pointer>(__match));
    }

    template <class KeyT>
    size_type __count_unique(const KeyT& __k) const;
    template <class KeyT>
    size_type __count_multi(const KeyT& __k) const;

    template <bool _LowerBound, class KeyT>
    __end_node_pointer __lower_upper_bound_unique_impl(const KeyT& __v) const {
        auto __rt     = __root();
        auto __result = __end_node();
        auto __comp   = LazySynthThreeWayComparator<_Compare, KeyT, value_type>(value_comp());
        while (__rt != nullptr) {
            auto __comp_res = __comp(__v, __rt->__get_value());

            if (__comp_res.__less()) {
                __result = static_cast<__end_node_pointer>(__rt);
                __rt     = static_cast<__node_pointer>(__rt->__left_);
            } else if (__comp_res.__greater()) {
                __rt = static_cast<__node_pointer>(__rt->__right_);
            } else if constexpr (_LowerBound) {
                return static_cast<__end_node_pointer>(__rt);
            } else {
                return __rt->__right_ ? static_cast<__end_node_pointer>(mstd::__tree_min(__rt->__right_)) : __result;
            }
        }
        return __result;
    }

    template <class KeyT>
    iterator __lower_bound_unique(const KeyT& __v) {
        return iterator(__lower_upper_bound_unique_impl<true>(__v));
    }

    template <class KeyT>
    const_iterator __lower_bound_unique(const KeyT& __v) const {
        return const_iterator(__lower_upper_bound_unique_impl<true>(__v));
    }

    template <class KeyT>
    iterator __upper_bound_unique(const KeyT& __v) {
        return iterator(__lower_upper_bound_unique_impl<false>(__v));
    }

    template <class KeyT>
    const_iterator __upper_bound_unique(const KeyT& __v) const {
        return iterator(__lower_upper_bound_unique_impl<false>(__v));
    }

private:
    template <class KeyT>
    iterator
    __lower_bound_multi(const KeyT& __v, __node_pointer __root, __end_node_pointer __result);

    template <class KeyT>
    const_iterator
    __lower_bound_multi(const KeyT& __v, __node_pointer __root, __end_node_pointer __result) const;

public:
    template <class KeyT>
    iterator __lower_bound_multi(const KeyT& __v) {
        return __lower_bound_multi(__v, __root(), __end_node());
    }
    template <class KeyT>
    const_iterator __lower_bound_multi(const KeyT& __v) const {
        return __lower_bound_multi(__v, __root(), __end_node());
    }

    template <class KeyT>
    iterator __upper_bound_multi(const KeyT& __v) {
        return __upper_bound_multi(__v, __root(), __end_node());
    }

    template <class KeyT>
    const_iterator __upper_bound_multi(const KeyT& __v) const {
        return __upper_bound_multi(__v, __root(), __end_node());
    }

private:
    template <class KeyT>
    iterator
    __upper_bound_multi(const KeyT& __v, __node_pointer __root, __end_node_pointer __result);

    template <class KeyT>
    const_iterator
    __upper_bound_multi(const KeyT& __v, __node_pointer __root, __end_node_pointer __result) const;

public:
    template <class KeyT>
    std::pair<iterator, iterator> __equal_range_unique(const KeyT& __k);
    template <class KeyT>
    std::pair<const_iterator, const_iterator> __equal_range_unique(const KeyT& __k) const;

    template <class KeyT>
    std::pair<iterator, iterator> __equal_range_multi(const KeyT& __k);
    template <class KeyT>
    std::pair<const_iterator, const_iterator> __equal_range_multi(const KeyT& __k) const;

    using _Dp           = __tree_node_destructor<__node_allocator>;
    using __node_holder = std::unique_ptr<__node, _Dp>;

    __node_holder remove(const_iterator __p) noexcept;

    // FIXME: Make this function const qualified. Unfortunately doing so
    // breaks existing code which uses non-const callable comparators.
    template <class KeyT>
    std::pair<__end_node_pointer, __node_base_pointer&> __find_equal(const KeyT& __v);

    template <class KeyT>
    std::pair<__end_node_pointer, __node_base_pointer&> __find_equal(const KeyT& __v) const {
        return const_cast<Tree*>(this)->__find_equal(__v);
    }

    template <class KeyT>
    std::pair<__end_node_pointer, __node_base_pointer&>
    __find_equal(const_iterator __hint, __node_base_pointer& __dummy, const KeyT& __v);

    void __copy_assign_alloc(const Tree& __t) {
        __copy_assign_alloc(__t, std::integral_constant<bool, __node_traits::propagate_on_container_copy_assignment::value>());
    }

    void __copy_assign_alloc(const Tree& __t, std::true_type) {
        if (__node_alloc() != __t.__node_alloc())
            clear();
        __node_alloc() = __t.__node_alloc();
    }
    void __copy_assign_alloc(const Tree&, std::false_type) {}

private:
    __node_base_pointer& __find_leaf_low(__end_node_pointer& __parent, const value_type& __v);

    __node_base_pointer& __find_leaf_high(__end_node_pointer& __parent, const value_type& __v);

    __node_base_pointer&
    __find_leaf(const_iterator __hint, __end_node_pointer& __parent, const value_type& __v);

    template <class... _Args>
    __node_holder __construct_node(_Args&&... __args);

    void destroy(__node_pointer __nd) noexcept { (__tree_deleter(__node_alloc_))(__nd); }

    void __move_assign(Tree& __t, std::false_type);
    void __move_assign(Tree& __t, std::true_type) noexcept(
        std::is_nothrow_move_assignable<value_compare>::value && std::is_nothrow_move_assignable<__node_allocator>::value);

    void __move_assign_alloc(Tree& __t)
    noexcept(!__node_traits::propagate_on_container_move_assignment::value ||
    std::is_nothrow_move_assignable<__node_allocator>::value) {
        __move_assign_alloc(__t, std::integral_constant<bool, __node_traits::propagate_on_container_move_assignment::value>());
    }

    void __move_assign_alloc(Tree& __t, std::true_type)
    noexcept(std::is_nothrow_move_assignable<__node_allocator>::value) {
        __node_alloc() = std::move(__t.__node_alloc());
    }
    void __move_assign_alloc(Tree&, std::false_type) noexcept {}

    template <class _From, class _ValueT = _Tp, std::enable_if_t<__is_tree_value_type_v<_ValueT>, int> = 0>
    static void __assign_value(__get_node_value_type_t<value_type>& __lhs, _From&& __rhs) {
        using __key_type = std::remove_const_t<typename value_type::first_type>;

        // This is technically UB, since the object was constructed as `const`.
        // Clang doesn't optimize on this currently though.
        const_cast<__key_type&>(__lhs.first) = const_cast<__copy_cvref_t<_From, __key_type>&&>(__rhs.first);
        __lhs.second                         = std::forward<_From>(__rhs).second;
    }

    template <class _To, class _From, class _ValueT = _Tp, std::enable_if_t<!__is_tree_value_type_v<_ValueT>, int> = 0>
    static void __assign_value(_To& __lhs, _From&& __rhs) {
        __lhs = std::forward<_From>(__rhs);
    }

    class __tree_deleter {
        __node_allocator& __alloc_;

    public:
        using pointer = __node_pointer;

        __tree_deleter(__node_allocator& __alloc) : __alloc_(__alloc) {}

        void
        operator()(__node_pointer __ptr) {
            if (!__ptr)
                return;

            (*this)(static_cast<__node_pointer>(__ptr->__left_));

            auto __right = __ptr->__right_;

            __node_traits::destroy(__alloc_, std::addressof(__ptr->__get_value()));
            __node_traits::deallocate(__alloc_, __ptr, 1);

            (*this)(static_cast<__node_pointer>(__right));
        }
    };

    // This copy construction will always produce a correct red-black-tree assuming the incoming tree is correct, since we
    // copy the exact structure 1:1. Since this is for copy construction _only_ we know that we get a correct tree. If we
    // didn't get a correct tree, the invariants of Tree are broken and we have a much bigger problem than an improperly
    // balanced tree.
    template <class _NodeConstructor>
    __node_pointer __construct_from_tree(__node_pointer __src, _NodeConstructor __construct) {
        if (!__src)
            return nullptr;

        __node_holder __new_node = __construct(__src->__get_value());

        std::unique_ptr<__node, __tree_deleter> __left(
            __construct_from_tree(static_cast<__node_pointer>(__src->__left_), __construct), __node_alloc_);
        __node_pointer __right = __construct_from_tree(static_cast<__node_pointer>(__src->__right_), __construct);

        __node_pointer __new_node_ptr = __new_node.release();

        __new_node_ptr->__is_black_ = __src->__is_black_;
        __new_node_ptr->__left_     = static_cast<__node_base_pointer>(__left.release());
        __new_node_ptr->__right_    = static_cast<__node_base_pointer>(__right);
        if (__new_node_ptr->__left_)
            __new_node_ptr->__left_->__parent_ = static_cast<__end_node_pointer>(__new_node_ptr);
        if (__new_node_ptr->__right_)
            __new_node_ptr->__right_->__parent_ = static_cast<__end_node_pointer>(__new_node_ptr);
        return __new_node_ptr;
    }

    __node_pointer __copy_construct_tree(__node_pointer __src) {
        return __construct_from_tree(__src, [this](const value_type& __val) { return __construct_node(__val); });
    }

    template <class _ValueT = _Tp, std::enable_if_t<__is_tree_value_type_v<_ValueT>, int> = 0>
    __node_pointer __move_construct_tree(__node_pointer __src) {
        return __construct_from_tree(__src, [this](value_type& __val) {
            return __construct_node(const_cast<key_type&&>(__val.first), std::move(__val.second));
        });
    }

    template <class _ValueT = _Tp, std::enable_if_t<!__is_tree_value_type_v<_ValueT>, int> = 0>
    __node_pointer __move_construct_tree(__node_pointer __src) {
        return __construct_from_tree(__src, [this](value_type& __val) { return __construct_node(std::move(__val)); });
    }

    template <class _Assignment, class _ConstructionAlg>
    // This copy assignment will always produce a correct red-black-tree assuming the incoming tree is correct, since our
    // own tree is a red-black-tree and the incoming tree is a red-black-tree. The invariants of a red-black-tree are
    // temporarily not met until all of the incoming red-black tree is copied.
    __node_pointer __assign_from_tree(
        __node_pointer __dest, __node_pointer __src, _Assignment __assign, _ConstructionAlg __construct_subtree) {
        if (!__src) {
            destroy(__dest);
            return nullptr;
        }

        __assign(__dest->__get_value(), __src->__get_value());
        __dest->__is_black_ = __src->__is_black_;

        // If we already have a left node in the destination tree, reuse it and copy-assign recursively
        if (__dest->__left_) {
            __dest->__left_ = static_cast<__node_base_pointer>(__assign_from_tree(
                static_cast<__node_pointer>(__dest->__left_),
                                                                                  static_cast<__node_pointer>(__src->__left_),
                                                                                  __assign,
                                                                                  __construct_subtree));

            // Otherwise, we must create new nodes; copy-construct from here on
        } else if (__src->__left_) {
            auto __new_left       = __construct_subtree(static_cast<__node_pointer>(__src->__left_));
            __dest->__left_       = static_cast<__node_base_pointer>(__new_left);
            __new_left->__parent_ = static_cast<__end_node_pointer>(__dest);
        }

        // Identical to the left case above, just for the right nodes
        if (__dest->__right_) {
            __dest->__right_ = static_cast<__node_base_pointer>(__assign_from_tree(
                static_cast<__node_pointer>(__dest->__right_),
                                                                                   static_cast<__node_pointer>(__src->__right_),
                                                                                   __assign,
                                                                                   __construct_subtree));
        } else if (__src->__right_) {
            auto __new_right       = __construct_subtree(static_cast<__node_pointer>(__src->__right_));
            __dest->__right_       = static_cast<__node_base_pointer>(__new_right);
            __new_right->__parent_ = static_cast<__end_node_pointer>(__dest);
        }

        return __dest;
        }

        __node_pointer __copy_assign_tree(__node_pointer __dest, __node_pointer __src) {
            return __assign_from_tree(
                __dest,
                __src,
                [](value_type& __lhs, const value_type& __rhs) { __assign_value(__lhs, __rhs); },
                                      [this](__node_pointer __nd) { return __copy_construct_tree(__nd); });
        }

        __node_pointer __move_assign_tree(__node_pointer __dest, __node_pointer __src) {
            return __assign_from_tree(
                __dest,
                __src,
                [](value_type& __lhs, value_type& __rhs) { __assign_value(__lhs, std::move(__rhs)); },
                                      [this](__node_pointer __nd) { return __move_construct_tree(__nd); });
        }

        friend struct __specialized_algorithm<_Algorithm::__for_each, __single_range<Tree> >;
};

template <class _Tp, class _Compare, class _Allocator>
struct __specialized_algorithm<_Algorithm::__for_each, __single_range<Tree<_Tp, _Compare, _Allocator> > > {
    static const bool __has_algorithm = true;

    using __node_pointer = typename Tree<_Tp, _Compare, _Allocator>::__node_pointer;

    template <class _Tree, class _Func, class _Proj>
    static auto operator()(_Tree&& __range, _Func __func, _Proj __proj) {
        if (__range.size() != 0)
            mstd::__tree_iterate_from_root<__copy_cvref_t<_Tree, typename std::remove_cvref_t<_Tree>::value_type>>(
                [](__node_pointer) { return false; }, __range.__root(), __func, __proj);
            return std::make_pair(__range.end(), std::move(__func));
    }
};

template <class _Tp, class _Compare, class _Allocator>
Tree<_Tp, _Compare, _Allocator>& Tree<_Tp, _Compare, _Allocator>::operator=(const Tree& __t) {
    if (this == std::addressof(__t))
        return *this;

    value_comp() = __t.value_comp();
    __copy_assign_alloc(__t);

    if (__size_ != 0) {
        *__root_ptr() = static_cast<__node_base_pointer>(__copy_assign_tree(__root(), __t.__root()));
    } else {
        *__root_ptr() = static_cast<__node_base_pointer>(__copy_construct_tree(__t.__root()));
        if (__root())
            __root()->__parent_ = __end_node();
    }
    __begin_node_ =
    __end_node()->__left_ ? static_cast<__end_node_pointer>(mstd::__tree_min(__end_node()->__left_)) : __end_node();
    __size_ = __t.size();

    return *this;
}

template <class _Tp, class _Compare, class _Allocator>
Tree<_Tp, _Compare, _Allocator>::Tree(const Tree& __t)
: __begin_node_(__end_node()),
__node_alloc_(__node_traits::select_on_container_copy_construction(__t.__node_alloc())),
__size_(0),
__value_comp_(__t.value_comp()) {
    if (__t.size() == 0)
        return;

    *__root_ptr()       = static_cast<__node_base_pointer>(__copy_construct_tree(__t.__root()));
    __root()->__parent_ = __end_node();
    __begin_node_       = static_cast<__end_node_pointer>(mstd::__tree_min(__end_node()->__left_));
    __size_             = __t.size();
}

template <class _Tp, class _Compare, class _Allocator>
Tree<_Tp, _Compare, _Allocator>::Tree(Tree&& __t) noexcept(
    std::is_nothrow_move_constructible<__node_allocator>::value&& std::is_nothrow_move_constructible<value_compare>::value)
: __begin_node_(std::move(__t.__begin_node_)),
__end_node_(std::move(__t.__end_node_)),
__node_alloc_(std::move(__t.__node_alloc_)),
__size_(__t.__size_),
__value_comp_(std::move(__t.__value_comp_)) {
    if (__size_ == 0)
        __begin_node_ = __end_node();
    else {
        __end_node()->__left_->__parent_ = static_cast<__end_node_pointer>(__end_node());
        __t.__begin_node_                = __t.__end_node();
        __t.__end_node()->__left_        = nullptr;
        __t.__size_                      = 0;
    }
}

template <class _Tp, class _Compare, class _Allocator>
Tree<_Tp, _Compare, _Allocator>::Tree(Tree&& __t, const allocator_type& __a)
: __begin_node_(__end_node()),
__node_alloc_(__node_allocator(__a)),
__size_(0),
__value_comp_(std::move(__t.value_comp())) {
    if (__t.size() == 0)
        return;
    if (__a == __t.__alloc()) {
        __begin_node_                    = __t.__begin_node_;
        __end_node()->__left_            = __t.__end_node()->__left_;
        __end_node()->__left_->__parent_ = static_cast<__end_node_pointer>(__end_node());
        __size_                          = __t.__size_;
        __t.__begin_node_                = __t.__end_node();
        __t.__end_node()->__left_        = nullptr;
        __t.__size_                      = 0;
    } else {
        *__root_ptr()       = static_cast<__node_base_pointer>(__move_construct_tree(__t.__root()));
        __root()->__parent_ = __end_node();
        __begin_node_       = static_cast<__end_node_pointer>(mstd::__tree_min(__end_node()->__left_));
        __size_             = __t.size();
        __t.clear(); // Ensure that __t is in a valid state after moving out the keys
    }
}

template <class _Tp, class _Compare, class _Allocator>
void Tree<_Tp, _Compare, _Allocator>::__move_assign(Tree& __t, std::true_type)
noexcept(std::is_nothrow_move_assignable<value_compare>::value && std::is_nothrow_move_assignable<__node_allocator>::value) {
    destroy(static_cast<__node_pointer>(__end_node()->__left_));
    __begin_node_ = __t.__begin_node_;
    __end_node_   = __t.__end_node_;
    __move_assign_alloc(__t);
    __size_       = __t.__size_;
    __value_comp_ = std::move(__t.__value_comp_);
    if (__size_ == 0)
        __begin_node_ = __end_node();
    else {
        __end_node()->__left_->__parent_ = static_cast<__end_node_pointer>(__end_node());
        __t.__begin_node_                = __t.__end_node();
        __t.__end_node()->__left_        = nullptr;
        __t.__size_                      = 0;
    }
}

template <class _Tp, class _Compare, class _Allocator>
void Tree<_Tp, _Compare, _Allocator>::__move_assign(Tree& __t, std::false_type) {
    if (__node_alloc() == __t.__node_alloc()) {
        __move_assign(__t, std::true_type());
    } else {
        value_comp() = std::move(__t.value_comp());
        if (__size_ != 0) {
            *__root_ptr() = static_cast<__node_base_pointer>(__move_assign_tree(__root(), __t.__root()));
        } else {
            *__root_ptr() = static_cast<__node_base_pointer>(__move_construct_tree(__t.__root()));
            if (__root())
                __root()->__parent_ = __end_node();
        }
        __begin_node_ =
        __end_node()->__left_ ? static_cast<__end_node_pointer>(mstd::__tree_min(__end_node()->__left_)) : __end_node();
        __size_ = __t.size();
        __t.clear(); // Ensure that __t is in a valid state after moving out the keys
    }
}

template <class _Tp, class _Compare, class _Allocator>
void Tree<_Tp, _Compare, _Allocator>::swap(Tree& __t)
noexcept(std::is_nothrow_swappable_v<value_compare>)
{
    using std::swap;
    swap(__begin_node_, __t.__begin_node_);
    swap(__end_node_, __t.__end_node_);
    mstd::__swap_allocator(__node_alloc(), __t.__node_alloc());
    swap(__size_, __t.__size_);
    swap(__value_comp_, __t.__value_comp_);
    if (__size_ == 0)
        __begin_node_ = __end_node();
    else
        __end_node()->__left_->__parent_ = __end_node();
    if (__t.__size_ == 0)
        __t.__begin_node_ = __t.__end_node();
    else
        __t.__end_node()->__left_->__parent_ = __t.__end_node();
}

template <class _Tp, class _Compare, class _Allocator>
void Tree<_Tp, _Compare, _Allocator>::clear() noexcept {
    destroy(__root());
    __size_               = 0;
    __begin_node_         = __end_node();
    __end_node()->__left_ = nullptr;
}

// Find lower_bound place to insert
// Set __parent to parent of null leaf
// Return reference to null leaf
template <class _Tp, class _Compare, class _Allocator>
typename Tree<_Tp, _Compare, _Allocator>::__node_base_pointer&
Tree<_Tp, _Compare, _Allocator>::__find_leaf_low(__end_node_pointer& __parent, const value_type& __v) {
    __node_pointer __nd = __root();
    if (__nd != nullptr) {
        while (true) {
            if (value_comp()(__nd->__get_value(), __v)) {
                if (__nd->__right_ != nullptr)
                    __nd = static_cast<__node_pointer>(__nd->__right_);
                else {
                    __parent = static_cast<__end_node_pointer>(__nd);
                    return __nd->__right_;
                }
            } else {
                if (__nd->__left_ != nullptr)
                    __nd = static_cast<__node_pointer>(__nd->__left_);
                else {
                    __parent = static_cast<__end_node_pointer>(__nd);
                    return __parent->__left_;
                }
            }
        }
    }
    __parent = __end_node();
    return __parent->__left_;
}

// Find upper_bound place to insert
// Set __parent to parent of null leaf
// Return reference to null leaf
template <class _Tp, class _Compare, class _Allocator>
typename Tree<_Tp, _Compare, _Allocator>::__node_base_pointer&
Tree<_Tp, _Compare, _Allocator>::__find_leaf_high(__end_node_pointer& __parent, const value_type& __v) {
    __node_pointer __nd = __root();
    if (__nd != nullptr) {
        while (true) {
            if (value_comp()(__v, __nd->__get_value())) {
                if (__nd->__left_ != nullptr)
                    __nd = static_cast<__node_pointer>(__nd->__left_);
                else {
                    __parent = static_cast<__end_node_pointer>(__nd);
                    return __parent->__left_;
                }
            } else {
                if (__nd->__right_ != nullptr)
                    __nd = static_cast<__node_pointer>(__nd->__right_);
                else {
                    __parent = static_cast<__end_node_pointer>(__nd);
                    return __nd->__right_;
                }
            }
        }
    }
    __parent = __end_node();
    return __parent->__left_;
}

// Find leaf place to insert closest to __hint
// First check prior to __hint.
// Next check after __hint.
// Next do O(log N) search.
// Set __parent to parent of null leaf
// Return reference to null leaf
template <class _Tp, class _Compare, class _Allocator>
typename Tree<_Tp, _Compare, _Allocator>::__node_base_pointer& Tree<_Tp, _Compare, _Allocator>::__find_leaf(
    const_iterator __hint, __end_node_pointer& __parent, const value_type& __v) {
    if (__hint == end() || !value_comp()(*__hint, __v)) // check before
    {
        // __v <= *__hint
        const_iterator __prior = __hint;
        if (__prior == begin() || !value_comp()(__v, *--__prior)) {
            // *prev(__hint) <= __v <= *__hint
            if (__hint.__ptr_->__left_ == nullptr) {
                __parent = static_cast<__end_node_pointer>(__hint.__ptr_);
                return __parent->__left_;
            } else {
                __parent = static_cast<__end_node_pointer>(__prior.__ptr_);
                return static_cast<__node_base_pointer>(__prior.__ptr_)->__right_;
            }
        }
        // __v < *prev(__hint)
        return __find_leaf_high(__parent, __v);
    }
    // else __v > *__hint
    return __find_leaf_low(__parent, __v);
    }

    // Find __v
    // If __v exists, return the parent of the node of __v and a reference to the pointer to the node of __v.
    // If __v doesn't exist, return the parent of the null leaf and a reference to the pointer to the null leaf.
    template <class _Tp, class _Compare, class _Allocator>
    template <class KeyT>
    std::pair<typename Tree<_Tp, _Compare, _Allocator>::__end_node_pointer,
typename Tree<_Tp, _Compare, _Allocator>::__node_base_pointer&>
Tree<_Tp, _Compare, _Allocator>::__find_equal(const KeyT& __v) {
    using _Pair = std::pair<__end_node_pointer, __node_base_pointer&>;

    __node_pointer __nd = __root();

    if (__nd == nullptr) {
        auto __end = __end_node();
        return _Pair(__end, __end->__left_);
    }

    __node_base_pointer* __node_ptr = __root_ptr();
    auto&& __transparent            = mstd::__as_transparent(value_comp());
    auto __comp = LazySynthThreeWayComparator<__make_transparent_t<_Compare>, KeyT, value_type>(__transparent);

    while (true) {
        auto __comp_res = __comp(__v, __nd->__get_value());

        if (__comp_res.__less()) {
            if (__nd->__left_ == nullptr)
                return _Pair(static_cast<__end_node_pointer>(__nd), __nd->__left_);

            __node_ptr = std::addressof(__nd->__left_);
            __nd       = static_cast<__node_pointer>(__nd->__left_);
        } else if (__comp_res.__greater()) {
            if (__nd->__right_ == nullptr)
                return _Pair(static_cast<__end_node_pointer>(__nd), __nd->__right_);

            __node_ptr = std::addressof(__nd->__right_);
            __nd       = static_cast<__node_pointer>(__nd->__right_);
        } else {
            return _Pair(static_cast<__end_node_pointer>(__nd), *__node_ptr);
        }
    }
}

// Find __v
// First check prior to __hint.
// Next check after __hint.
// Next do O(log N) search.
// If __v exists, return the parent of the node of __v and a reference to the pointer to the node of __v.
// If __v doesn't exist, return the parent of the null leaf and a reference to the pointer to the null leaf.
template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
std::pair<typename Tree<_Tp, _Compare, _Allocator>::__end_node_pointer,
typename Tree<_Tp, _Compare, _Allocator>::__node_base_pointer&>
Tree<_Tp, _Compare, _Allocator>::__find_equal(const_iterator __hint, __node_base_pointer& __dummy, const KeyT& __v) {
    using _Pair = std::pair<__end_node_pointer, __node_base_pointer&>;

    if (__hint == end() || value_comp()(__v, *__hint)) { // check before
        // __v < *__hint
        const_iterator __prior = __hint;
        if (__prior == begin() || value_comp()(*--__prior, __v)) {
            // *prev(__hint) < __v < *__hint
            if (__hint.__ptr_->__left_ == nullptr)
                return _Pair(__hint.__ptr_, __hint.__ptr_->__left_);
            return _Pair(__prior.__ptr_, static_cast<__node_pointer>(__prior.__ptr_)->__right_);
        }
        // __v <= *prev(__hint)
        return __find_equal(__v);
    }

    if (value_comp()(*__hint, __v)) { // check after
        // *__hint < __v
        const_iterator __next = std::next(__hint);
        if (__next == end() || value_comp()(__v, *__next)) {
            // *__hint < __v < *std::next(__hint)
            if (__hint.__get_np()->__right_ == nullptr)
                return _Pair(__hint.__ptr_, static_cast<__node_pointer>(__hint.__ptr_)->__right_);
            return _Pair(__next.__ptr_, __next.__ptr_->__left_);
        }
        // *next(__hint) <= __v
        return __find_equal(__v);
    }

    // else __v == *__hint
    __dummy = static_cast<__node_base_pointer>(__hint.__ptr_);
    return _Pair(__hint.__ptr_, __dummy);
}

template <class _Tp, class _Compare, class _Allocator>
void Tree<_Tp, _Compare, _Allocator>::__insert_node_at(
    __end_node_pointer __parent, __node_base_pointer& __child, __node_base_pointer __new_node) noexcept {
        __new_node->__left_   = nullptr;
        __new_node->__right_  = nullptr;
        __new_node->__parent_ = __parent;
        // __new_node->__is_black_ is initialized in __tree_balance_after_insert
        __child = __new_node;
        if (__begin_node_->__left_ != nullptr)
            __begin_node_ = static_cast<__end_node_pointer>(__begin_node_->__left_);
        mstd::__tree_balance_after_insert(__end_node()->__left_, __child);
        ++__size_;
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class... _Args>
    typename Tree<_Tp, _Compare, _Allocator>::__node_holder
    Tree<_Tp, _Compare, _Allocator>::__construct_node(_Args&&... __args) {
        __node_allocator& __na = __node_alloc();
        __node_holder __h(__node_traits::allocate(__na, 1), _Dp(__na));
        std::construct_at(std::addressof(*__h), __na, std::forward<_Args>(__args)...);
        __h.get_deleter().__value_constructed = true;
        return __h;
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class... _Args>
    typename Tree<_Tp, _Compare, _Allocator>::iterator
    Tree<_Tp, _Compare, _Allocator>::__emplace_multi(_Args&&... __args) {
        __node_holder __h = __construct_node(std::forward<_Args>(__args)...);
        __end_node_pointer __parent;
        __node_base_pointer& __child = __find_leaf_high(__parent, __h->__get_value());
        __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__h.get()));
        return iterator(static_cast<__node_pointer>(__h.release()));
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class... _Args>
    typename Tree<_Tp, _Compare, _Allocator>::iterator
    Tree<_Tp, _Compare, _Allocator>::__emplace_hint_multi(const_iterator __p, _Args&&... __args) {
        __node_holder __h = __construct_node(std::forward<_Args>(__args)...);
        __end_node_pointer __parent;
        __node_base_pointer& __child = __find_leaf(__p, __parent, __h->__get_value());
        __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__h.get()));
        return iterator(static_cast<__node_pointer>(__h.release()));
    }

    template <class _Tp, class _Compare, class _Allocator>
    typename Tree<_Tp, _Compare, _Allocator>::iterator
    Tree<_Tp, _Compare, _Allocator>::__remove_node_pointer(__node_pointer __ptr) noexcept {
        iterator __r(__ptr);
        ++__r;
        if (__begin_node_ == __ptr)
            __begin_node_ = __r.__ptr_;
        --__size_;
        mstd::__tree_remove(__end_node()->__left_, static_cast<__node_base_pointer>(__ptr));
        return __r;
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class _NodeHandle, class _InsertReturnType>
    _InsertReturnType
    Tree<_Tp, _Compare, _Allocator>::__node_handle_insert_unique(_NodeHandle&& __nh) {
        if (__nh.empty())
            return _InsertReturnType{end(), false, _NodeHandle()};

        __node_pointer __ptr = __nh.__ptr_;
        auto [__parent, __child] = __find_equal(__ptr->__get_value());
        if (__child != nullptr)
            return _InsertReturnType{iterator(static_cast<__node_pointer>(__child)), false, std::move(__nh)};

        __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__ptr));
        __nh.__release_ptr();
        return _InsertReturnType{iterator(__ptr), true, _NodeHandle()};
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class _NodeHandle>
    typename Tree<_Tp, _Compare, _Allocator>::iterator
    Tree<_Tp, _Compare, _Allocator>::__node_handle_insert_unique(const_iterator __hint, _NodeHandle&& __nh) {
        if (__nh.empty())
            return end();

        __node_pointer __ptr = __nh.__ptr_;
        __node_base_pointer __dummy;
        auto [__parent, __child] = __find_equal(__hint, __dummy, __ptr->__get_value());
        __node_pointer __r       = static_cast<__node_pointer>(__child);
        if (__child == nullptr) {
            __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__ptr));
            __r = __ptr;
            __nh.__release_ptr();
        }
        return iterator(__r);
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class _NodeHandle>
    _NodeHandle Tree<_Tp, _Compare, _Allocator>::__node_handle_extract(key_type const& __key) {
        iterator __it = find(__key);
        if (__it == end())
            return _NodeHandle();
        return __node_handle_extract<_NodeHandle>(__it);
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class _NodeHandle>
    _NodeHandle Tree<_Tp, _Compare, _Allocator>::__node_handle_extract(const_iterator __p) {
        __node_pointer __np = __p.__get_np();
        __remove_node_pointer(__np);
        return _NodeHandle(__np, __alloc());
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class _Comp2>
    void
    Tree<_Tp, _Compare, _Allocator>::__node_handle_merge_unique(Tree<_Tp, _Comp2, _Allocator>& __source) {
        for (iterator __i = __source.begin(); __i != __source.end();) {
            __node_pointer __src_ptr = __i.__get_np();
            auto [__parent, __child] = __find_equal(__src_ptr->__get_value());
            ++__i;
            if (__child != nullptr)
                continue;
            __source.__remove_node_pointer(__src_ptr);
            __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__src_ptr));
        }
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class _NodeHandle>
    typename Tree<_Tp, _Compare, _Allocator>::iterator
    Tree<_Tp, _Compare, _Allocator>::__node_handle_insert_multi(_NodeHandle&& __nh) {
        if (__nh.empty())
            return end();
        __node_pointer __ptr = __nh.__ptr_;
        __end_node_pointer __parent;
        __node_base_pointer& __child = __find_leaf_high(__parent, __ptr->__get_value());
        __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__ptr));
        __nh.__release_ptr();
        return iterator(__ptr);
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class _NodeHandle>
    typename Tree<_Tp, _Compare, _Allocator>::iterator
    Tree<_Tp, _Compare, _Allocator>::__node_handle_insert_multi(const_iterator __hint, _NodeHandle&& __nh) {
        if (__nh.empty())
            return end();

        __node_pointer __ptr = __nh.__ptr_;
        __end_node_pointer __parent;
        __node_base_pointer& __child = __find_leaf(__hint, __parent, __ptr->__get_value());
        __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__ptr));
        __nh.__release_ptr();
        return iterator(__ptr);
    }

    template <class _Tp, class _Compare, class _Allocator>
    template <class _Comp2>
    void
    Tree<_Tp, _Compare, _Allocator>::__node_handle_merge_multi(Tree<_Tp, _Comp2, _Allocator>& __source) {
        for (iterator __i = __source.begin(); __i != __source.end();) {
            __node_pointer __src_ptr = __i.__get_np();
            __end_node_pointer __parent;
            __node_base_pointer& __child = __find_leaf_high(__parent, __src_ptr->__get_value());
            ++__i;
            __source.__remove_node_pointer(__src_ptr);
            __insert_node_at(__parent, __child, static_cast<__node_base_pointer>(__src_ptr));
        }
    }

template <class _Tp, class _Compare, class _Allocator>
auto Tree<_Tp, _Compare, _Allocator>::erase(const_iterator pos) -> iterator {
    auto node_ptr        = pos.__get_np();
    auto ret             = __remove_node_pointer(node_ptr);
    auto& node_allocator = __node_alloc();
    __node_traits::destroy(node_allocator, std::addressof(const_cast<value_type&>(*pos)));
    __node_traits::deallocate(node_allocator, node_ptr, 1);
    return ret;
}

template <class _Tp, class _Compare, class _Allocator>
auto Tree<_Tp, _Compare, _Allocator>::erase(const_iterator begin, const_iterator end) -> iterator {
    while (begin != end) {
        begin = erase(begin);
    }
    return iterator(end.__ptr_);
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__erase_unique(const KeyT& key) -> size_type {
    if (const auto iter = find(key); iter != end()) {
        erase(iter);
        return 1;
    }
    return 0;
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__erase_multi(const KeyT& key) -> size_type {
    auto [begin, end] = __equal_range_multi(key);
    size_type ret_cnt = 0;
    for (; begin != end; ++ret_cnt) {
        begin = erase(begin);
    }
    return ret_cnt;
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__count_unique(const KeyT& key) const
  -> size_type {
    auto root = __root();
    auto comp         = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
    while (root != nullptr) {
        const auto comp_res = comp(key, root->__get_value());
        if (comp_res.__less()) {
            root = static_cast<__node_pointer>(root->__left_);
        } else if (comp_res.__greater()) {
            root = static_cast<__node_pointer>(root->__right_);
        } else {
            return 1;
        }
    }
    return 0;
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__count_multi(const KeyT& key) const
  -> size_type {
    auto result = __end_node();
    auto root   = __root();
    auto comp   = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
    while (root != nullptr) {
        const auto comp_res = comp(key, root->__get_value());
        if (comp_res.__less()) {
            result = static_cast<__end_node_pointer>(root);
            root   = static_cast<__node_pointer>(root->__left_);
        } else if (comp_res.__greater()) {
            root = static_cast<__node_pointer>(root->__right_);
        } else {
            return std::distance(
                __lower_bound_multi(key, static_cast<__node_pointer>(root->__left_), static_cast<__end_node_pointer>(root)),
                __upper_bound_multi(key, static_cast<__node_pointer>(root->__right_), result)
            );
        }
    }
    return 0;
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__lower_bound_multi(
    const KeyT& key, __node_pointer root, __end_node_pointer result) 
  -> iterator {
    while (root != nullptr) {
        if (!value_comp()(root->__get_value(), key)) {
            result = static_cast<__end_node_pointer>(root);
            root   = static_cast<__node_pointer>(root->__left_);
        } else {
            root = static_cast<__node_pointer>(root->__right_);
        }
    }
    return iterator(result);
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__lower_bound_multi(
    const KeyT& key, __node_pointer root, __end_node_pointer result) const 
  -> const_iterator {
    while (root != nullptr) {
        if (!value_comp()(root->__get_value(), key)) {
            result = static_cast<__end_node_pointer>(root);
            root   = static_cast<__node_pointer>(root->__left_);
        } else {
            root = static_cast<__node_pointer>(root->__right_);
        }
    }
    return const_iterator(result);
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__upper_bound_multi(
    const KeyT& key, __node_pointer root, __end_node_pointer result) 
  -> iterator {
    while (root != nullptr) {
        if (value_comp()(key, root->__get_value())) {
            result = static_cast<__end_node_pointer>(root);
            root   = static_cast<__node_pointer>(root->__left_);
        } else {
            root = static_cast<__node_pointer>(root->__right_);
        }
    }
    return iterator(result);
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__upper_bound_multi(
    const KeyT& key, __node_pointer root, __end_node_pointer result) const
  -> const_iterator {
    while (root != nullptr) {
        if (value_comp()(key, root->__get_value())) {
            result = static_cast<__end_node_pointer>(root);
            root   = static_cast<__node_pointer>(root->__left_);
        } else {
            root = static_cast<__node_pointer>(root->__right_);
        }
    }
    return const_iterator(result);
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__equal_range_unique(const KeyT& key)
  -> std::pair<iterator, iterator> {
    auto result = __end_node();
    auto root   = __root();
    auto comp   = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
    while (root != nullptr) {
        const auto comp_res = comp(key, root->__get_value());
        if (comp_res.__less()) {
            result = static_cast<__end_node_pointer>(root);
            root   = static_cast<__node_pointer>(root->__left_);
        } else if (comp_res.__greater()) {
            root = static_cast<__node_pointer>(root->__right_);
        } else {
            return {
                iterator(root),
                iterator(
                    (root->__right_ != nullptr)
                    ? static_cast<__end_node_pointer>(mstd::__tree_min(root->__right_))
                    : result
                ),
            };
        }
    }
    return {
        iterator(result),
        iterator(result),
    };
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__equal_range_unique(const KeyT& key) const
  -> std::pair<const_iterator, const_iterator> {
    auto result = __end_node();
    auto root   = __root();
    auto comp   = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
    while (root != nullptr) {
        const auto comp_res = comp(key, root->__get_value());
        if (comp_res.__less()) {
            result = static_cast<__end_node_pointer>(root);
            root   = static_cast<__node_pointer>(root->__left_);
        } else if (comp_res.__greater()) {
            root = static_cast<__node_pointer>(root->__right_);
        } else {  // Equal
            return {
                const_iterator(root),
                const_iterator(
                    (root->__right_ != nullptr)
                    ? static_cast<__end_node_pointer>(mstd::__tree_min(root->__right_))
                    : result
                ),
            };
        }
    }
    return {
        const_iterator(result),
        const_iterator(result),
    };
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__equal_range_multi(const KeyT& key)
  -> std::pair<iterator, iterator> {
    auto result = __end_node();
    auto root   = __root();
    auto comp   = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
    while (root != nullptr) {
        const auto comp_res = comp(key, root->__get_value());
        if (comp_res.__less()) {
            result = static_cast<__end_node_pointer>(root);
            root     = static_cast<__node_pointer>(root->__left_);
        } else if (comp_res.__greater()) {
            root = static_cast<__node_pointer>(root->__right_);
        } else {  // Equal
            return {
                __lower_bound_multi(key, static_cast<__node_pointer>(root->__left_), static_cast<__end_node_pointer>(root)),
                __upper_bound_multi(key, static_cast<__node_pointer>(root->__right_), result)
            };
        }
    }
    return {
        iterator(result),
        iterator(result),
    };
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto
Tree<_Tp, _Compare, _Allocator>::__equal_range_multi(const KeyT& key) const
  -> std::pair<const_iterator, const_iterator> {
    auto result = __end_node();
    auto root   = __root();
    auto comp   = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
    while (root != nullptr) {
        const auto comp_res = comp(key, root->__get_value());
        if (comp_res.__less()) {
            result = static_cast<__end_node_pointer>(root);
            root   = static_cast<__node_pointer>(root->__left_);
        } else if (comp_res.__greater()) {
            root = static_cast<__node_pointer>(root->__right_);
        } else {  // Equal
            return {
                __lower_bound_multi(key, static_cast<__node_pointer>(root->__left_), static_cast<__end_node_pointer>(root)),
                __upper_bound_multi(key, static_cast<__node_pointer>(root->__right_), result),
            };
        }
    }
    return {
        const_iterator(result),
        const_iterator(result),
    };
}

template <class _Tp, class _Compare, class _Allocator>
typename Tree<_Tp, _Compare, _Allocator>::__node_holder
Tree<_Tp, _Compare, _Allocator>::remove(const_iterator __p) noexcept {
    __node_pointer __np = __p.__get_np();
    if (__begin_node_ == __p.__ptr_) {
        if (__np->__right_ != nullptr)
            __begin_node_ = static_cast<__end_node_pointer>(__np->__right_);
        else
            __begin_node_ = static_cast<__end_node_pointer>(__np->__parent_);
    }
    --__size_;
    mstd::__tree_remove(__end_node()->__left_, static_cast<__node_base_pointer>(__np));
    return __node_holder(__np, _Dp(__node_alloc(), true));
}

template <class _Tp, class _Compare, class _Allocator>
inline void swap(Tree<_Tp, _Compare, _Allocator>& __x, Tree<_Tp, _Compare, _Allocator>& __y)
noexcept(noexcept(__x.swap(__y))) {
    __x.swap(__y);
}

} // namespace mstd


#endif // MSTD___TREE
